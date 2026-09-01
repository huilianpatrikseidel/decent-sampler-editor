#include <QtTest>
#include <QObject>
#include <QCoreApplication>
#include <QFile>
#include <QString>

// Includes from core project
#include "core/models/AudioNodes.h"
#include "core/models/UiComponents.h"
#include "core/models/GraphStructures.h"
#include "core/ProjectManager.h"
#include "commands/AddNodeCommand.h"
#include "commands/MoveNodeCommand.h"
#include "commands/ModifyPropertyCommand.h"
#include "commands/RemoveNodeCommand.h"
#include "commands/ConnectionCommands.h"
#include "commands/UiCommands.h"
#include "commands/ZoneCommands.h"
#include "audio/AudioMessage.h"
#include "audio/AudioEngine.h"
#include "audio/dsp/VoiceProcessor.h"
#include <concurrentqueue.h>
#include <QTemporaryDir>
#include <QDataStream>
#include <QJsonDocument>
#include "ui/canvas/FilmstripRenderer.h"
#include "ui/canvas/UiComponentItem.h"
#include "ui/waveform/WaveformCache.h"
#include "transpilers/DecentSamplerTranspiler.h"
#include "transpilers/ds/DsEffectBuilder.h"
#include "transpilers/SfzTranspiler.h"
#include "export/BundleExporter.h"
#include "export/FlacEncoder.h"
#include "core/DecibelUtils.h"
#include "ui/components/FaderWidget.h"
#include "core/ProjectSerializer.h"
#include <QSignalSpy>
#include <miniz.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>

class MainTest : public QObject {
    Q_OBJECT

private slots:
    // MemoryModelTest
    void testNodeCreationAndUUID();
    
    // UndoRedoTest
    void testAddNodeCommandLogic();
    
    // AudioEngineTest
    void testLockFreeQueueMessaging();
    void testAudioMessageDefaults();
    void testVoiceAdoptLifecycle();
    void testMidiModulationSources();
    void testAudioEngineSourceLifecycle();
    
    // ProjectManagerTest
    void testConnectionValidation();
    void testUndoRedoCommands();
    void testCascadingDeletion();
    void testRoundTripSerialization();
    
    // UIDesignerTest
    void testFilmstripMath();
    void testUiComponentSerialization();
    void testZoneCommands();
    void testWaveformCache();
    void testDecentSamplerTranspiler();
    void testSfzTranspiler();

    // RoutingTest
    void testBusRoutingPersists();
    void testBusRoutingRejectsCycles();

    // ExportChainTest
    void testGroupInsertChainOrderExported();
    void testEffectPositionSkipsBypassedAndCountsEqBands();

    // VolumeTest
    void testDecibelConversion();
    void testGroupVolumeExportedAsDecibels();

    // ExportTest
    void testFlacEncoderRoundTrip();
    void testFlacEncoderRejectsFloatWav();
    void testBundleIncludesWavetable();
};

namespace {

// Builds a RIFF/WAVE file around `samples`, which are signed and right-aligned to `bits`.
// 8-bit WAV data is unsigned on disk, so it is biased on the way out.
QByteArray buildWav(const std::vector<qint32>& samples, int channels, int sampleRate,
                    int bits, quint16 formatTag = 1) {
    const int bytesPerSample = bits / 8;
    QByteArray pcm;
    pcm.reserve(static_cast<int>(samples.size()) * bytesPerSample);
    for (qint32 sample : samples) {
        const quint32 v = (bits == 8) ? static_cast<quint32>(sample + 128)
                                      : static_cast<quint32>(sample);
        for (int b = 0; b < bytesPerSample; ++b) {
            pcm.append(static_cast<char>((v >> (8 * b)) & 0xFF));
        }
    }

    const quint32 dataSize = static_cast<quint32>(pcm.size());
    QByteArray wav;
    QDataStream s(&wav, QIODevice::WriteOnly);
    s.setByteOrder(QDataStream::LittleEndian);
    s.writeRawData("RIFF", 4); s << static_cast<quint32>(36 + dataSize); s.writeRawData("WAVE", 4);
    s.writeRawData("fmt ", 4);
    s << static_cast<quint32>(16)
      << formatTag
      << static_cast<quint16>(channels)
      << static_cast<quint32>(sampleRate)
      << static_cast<quint32>(sampleRate * channels * bytesPerSample)
      << static_cast<quint16>(channels * bytesPerSample)
      << static_cast<quint16>(bits);
    s.writeRawData("data", 4); s << dataSize; s.writeRawData(pcm.constData(), pcm.size());
    return wav;
}

// A signal built to hit every subframe path: a silent run (CONSTANT), a smooth tone
// (fixed predictors pay off), and white noise (they do not, so VERBATIM takes over).
std::vector<qint32> buildTestSignal(int frames, int channels, int bits) {
    const qint32 peak = (1 << (bits - 1)) - 1;
    std::vector<qint32> out(static_cast<size_t>(frames) * channels);
    quint32 rng = 12345u;
    for (int i = 0; i < frames; ++i) {
        rng = rng * 1664525u + 1013904223u;
        qint32 base;
        if (i < frames / 4) {
            base = 0;
        } else if (i < frames / 2) {
            const double t = static_cast<double>(i) / 44100.0;
            base = static_cast<qint32>(std::sin(2.0 * 3.14159265358979323846 * 220.0 * t) * peak * 0.9);
        } else {
            base = static_cast<qint32>(rng >> 8) % (peak + 1);
            if ((rng & 1u) != 0) base = -base;
        }
        for (int c = 0; c < channels; ++c) {
            // Channel 1 tracks channel 0 loosely, so stereo decorrelation has something to find.
            const qint32 v = (c == 0) ? base : base / 2 + 7;
            out[static_cast<size_t>(i) * channels + c] = std::clamp(v, -peak - 1, peak);
        }
    }
    return out;
}

// Decodes a FLAC stream back to signed samples right-aligned to `bits`.
bool decodeFlac(const QByteArray& flac, int bits, int channels, std::vector<qint32>& out) {
    ma_decoder_config config = ma_decoder_config_init(ma_format_s32, 0, 0);
    ma_decoder decoder;
    if (ma_decoder_init_memory(flac.constData(), static_cast<size_t>(flac.size()),
                               &config, &decoder) != MA_SUCCESS) {
        return false;
    }
    if (static_cast<int>(decoder.outputChannels) != channels) {
        ma_decoder_uninit(&decoder);
        return false;
    }

    ma_uint64 total = 0;
    ma_decoder_get_length_in_pcm_frames(&decoder, &total);
    out.resize(static_cast<size_t>(total) * channels);

    ma_uint64 done = 0;
    while (done < total) {
        ma_uint64 read = 0;
        ma_decoder_read_pcm_frames(&decoder, out.data() + done * channels, total - done, &read);
        if (read == 0) break;
        done += read;
    }
    ma_decoder_uninit(&decoder);

    out.resize(static_cast<size_t>(done) * channels);
    const int shift = 32 - bits;
    for (qint32& v : out) v >>= shift;
    return done == total && total > 0;
}

} // namespace

void MainTest::testNodeCreationAndUUID() {
    auto group = std::make_unique<SampleGroup>();
    group->name = "Test Group";
    
    QVERIFY(!group->id.isNull());
    QCOMPARE(group->type, QString("SampleGroup"));
    
    QJsonObject json = group->toJson();
    QCOMPARE(json["name"].toString(), QString("Test Group"));
    QCOMPARE(json["volume"].toDouble(), 0.0);
}

void MainTest::testAddNodeCommandLogic() {
    ProjectManager pm;
    auto node = std::make_unique<SampleGroup>();
    node->name = "Undo Node";
    QUuid id = node->id;
    
    pm.getUndoStack()->push(new AddNodeCommand(&pm, std::move(node), QPointF(0, 0)));
    QVERIFY(pm.getNode(id) != nullptr);
    
    pm.getUndoStack()->undo();
    QVERIFY(pm.getNode(id) == nullptr);
    
    pm.getUndoStack()->redo();
    QVERIFY(pm.getNode(id) != nullptr);
}

void MainTest::testLockFreeQueueMessaging() {
    moodycamel::ConcurrentQueue<AudioMessage> queue;
    queue.enqueue({AudioCommandType::PlayNote, 0.8f}); // Was PlayTestTone
    
    AudioMessage msg;
    bool result = queue.try_dequeue(msg);
    QVERIFY(result == true);
    QVERIFY(msg.type == AudioCommandType::PlayNote);
    QVERIFY(msg.value == 0.8f);
}

void MainTest::testAudioMessageDefaults() {
    // Every message must start with a null prepared source: the audio thread treats a
    // non-null value as a ready-to-play stream, so a stale/garbage pointer would crash.
    AudioMessage msg;
    QVERIFY(msg.preparedSource == nullptr);
    QVERIFY(msg.isOscillator == false);
    QCOMPARE(msg.numRoutings, 0);
}

void MainTest::testVoiceAdoptLifecycle() {
    VoiceProcessor voice;

    // releaseSource() on a fresh voice (nothing held) must be a safe no-op.
    voice.releaseSource();

    // adopt() must return the PREVIOUSLY held source so the audio thread can push it to
    // the free queue instead of leaking it. Sentinel pointers we never dereference/free.
    auto* a = reinterpret_cast<ma_resource_manager_data_source*>(0x100);
    auto* b = reinterpret_cast<ma_resource_manager_data_source*>(0x200);

    QVERIFY(voice.adopt(a) == nullptr);        // nothing was held
    QVERIFY(voice.adopt(b) == a);              // returns the source it was holding
    QVERIFY(voice.adopt(nullptr) == b);        // returns b and clears the slot
    QVERIFY(voice.adopt(nullptr) == nullptr);  // slot is now empty

    // Slot left empty on purpose: ~VoiceProcessor() must not uninit a sentinel pointer.
}

void MainTest::testMidiModulationSources() {
    // ModWheel / PitchBend / Aftertouch used to fall through the mod-source switch and
    // contribute nothing at all. Render an oscillator voice with each one routed to
    // Volume at full negative depth: at source 0 the voice sounds, at source 1 it is
    // fully attenuated. Anything that does not reach the switch leaves both equal.
    auto renderPeak = [](ModSource source, float sourceValue) {
        AudioMessage msg;
        msg.note = 69;
        msg.velocity = 127;
        msg.volume = 1.0f;
        msg.sustain = 1.0f;
        msg.isOscillator = true;
        msg.oscWaveform = 0; // sine
        msg.numRoutings = 1;
        msg.routings[0].source = source;
        msg.routings[0].dest = ModDest::Volume;
        msg.routings[0].amount = -1.0f;

        VoiceProcessor voice;
        voice.setSampleRate(44100.0);
        voice.trigger(msg, msg.note);

        ModInputs mods;
        switch (source) {
            case ModSource::ModWheel: mods.modWheel = sourceValue; break;
            case ModSource::PitchBend: mods.pitchBend = sourceValue; break;
            case ModSource::Aftertouch: mods.aftertouch = sourceValue; break;
            default: break;
        }

        float peak = 0.0f;
        for (int i = 0; i < 2048; ++i) {
            float l = 0.0f, r = 0.0f;
            voice.process(mods, l, r, nullptr);
            peak = std::max(peak, std::abs(l));
        }
        return peak;
    };

    const ModSource sources[] = { ModSource::ModWheel, ModSource::PitchBend, ModSource::Aftertouch };
    for (ModSource source : sources) {
        const float idle = renderPeak(source, 0.0f);
        const float driven = renderPeak(source, 1.0f);

        QVERIFY2(idle > 0.01f, "oscillator voice produced no signal at all");
        QVERIFY2(driven < idle * 0.01f, "mod source did not reach the voice");
    }
}

void MainTest::testAudioEngineSourceLifecycle() {
    AudioEngine engine;
    if (!engine.initialize(QString(), 44100, 0)) {
        QSKIP("No audio output device available in this environment.");
    }

    // Write a small but valid 16-bit mono PCM WAV to a temp directory.
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString wavPath = dir.filePath("tone.wav");
    {
        QByteArray pcm;
        {
            QDataStream ps(&pcm, QIODevice::WriteOnly);
            ps.setByteOrder(QDataStream::LittleEndian);
            for (int i = 0; i < 32; ++i) ps << static_cast<qint16>(i * 200);
        }
        const quint32 dataSize = static_cast<quint32>(pcm.size());
        QByteArray wav;
        QDataStream s(&wav, QIODevice::WriteOnly);
        s.setByteOrder(QDataStream::LittleEndian);
        s.writeRawData("RIFF", 4); s << static_cast<quint32>(36 + dataSize); s.writeRawData("WAVE", 4);
        s.writeRawData("fmt ", 4);
        s << static_cast<quint32>(16)         // fmt chunk size
          << static_cast<quint16>(1)          // PCM
          << static_cast<quint16>(1)          // mono
          << static_cast<quint32>(44100)      // sample rate
          << static_cast<quint32>(44100 * 2)  // byte rate
          << static_cast<quint16>(2)          // block align
          << static_cast<quint16>(16);        // bits per sample
        s.writeRawData("data", 4); s << dataSize; s.writeRawData(pcm.constData(), pcm.size());
        QFile f(wavPath);
        QVERIFY(f.open(QIODevice::WriteOnly));
        QCOMPARE(static_cast<int>(f.write(wav)), wav.size());
        f.close();
    }

    // prepareSampleSource opens a streaming source off the audio thread.
    void* handle = engine.prepareSampleSource(wavPath.toUtf8().constData());
    QVERIFY(handle != nullptr);

    // Dispose it through the free-queue path (uninits a real miniaudio source).
    engine.disposePreparedSource(handle);

    // reinitialize exercises releaseAllVoiceSources() before tearing down the resource
    // manager — the fix that stops voice sources dangling across a device change.
    engine.reinitialize(QString(), 44100, 0);

    // Prepare + dispose once more to confirm the engine is still usable after reinit.
    void* handle2 = engine.prepareSampleSource(wavPath.toUtf8().constData());
    engine.disposePreparedSource(handle2);

    QVERIFY(true); // reached teardown without crashing; ~AudioEngine disposes the rest
}

void MainTest::testConnectionValidation() {
    ProjectManager pm;
    auto sg = std::make_unique<SampleGroup>();
    auto bus = std::make_unique<BusNode>();
    QUuid sgId = sg->id;
    QUuid busId = bus->id;
    pm.addNode(std::move(sg));
    pm.addNode(std::move(bus));

    Connection validConn{sgId, "audio_out", busId, "audio_in"};
    QVERIFY(pm.canConnect(validConn));

    Connection invalidConn1{sgId, "audio_out", busId, "audio_out"}; 
    QVERIFY(!pm.canConnect(invalidConn1));
    
    Connection invalidConn2{sgId, "invalid_port", busId, "audio_in"}; 
    QVERIFY(!pm.canConnect(invalidConn2));
}

void MainTest::testUndoRedoCommands() {
    ProjectManager pm;
    auto sg = std::make_unique<SampleGroup>();
    QUuid sgId = sg->id;
    pm.addNode(std::move(sg));

    MoveNodeCommand move(&pm, sgId, QPointF(0,0), QPointF(100,50));
    move.redo();
    QCOMPARE(pm.getNodePosition(sgId).x(), 100.0);
    
    move.undo();
    QCOMPARE(pm.getNodePosition(sgId).x(), 0.0);
    
    ModifyPropertyCommand prop(&pm, sgId, "volume", 0.0, -5.0);
    prop.redo();
    QCOMPARE(static_cast<SampleGroup*>(pm.getNode(sgId))->volume, -5.0);
    
    prop.undo();
    QCOMPARE(static_cast<SampleGroup*>(pm.getNode(sgId))->volume, 0.0);
}

void MainTest::testCascadingDeletion() {
    ProjectManager pm;
    auto sg = std::make_unique<SampleGroup>();
    auto bus = std::make_unique<BusNode>();
    QUuid sgId = sg->id;
    QUuid busId = bus->id;
    pm.addNode(std::move(sg));
    pm.addNode(std::move(bus));

    Connection c{sgId, "audio_out", busId, "audio_in"};
    pm.addConnection(c);
    
    QCOMPARE(pm.getConnectionsForNode(sgId).size(), static_cast<size_t>(1));
    
    RemoveNodeCommand rm(&pm, sgId);
    rm.redo();
    
    QVERIFY(pm.getNode(sgId) == nullptr);
    QCOMPARE(pm.getConnectionsForNode(busId).size(), static_cast<size_t>(0));
    
    rm.undo();
    
    QVERIFY(pm.getNode(sgId) != nullptr);
    QCOMPARE(pm.getConnectionsForNode(busId).size(), static_cast<size_t>(1));
}

void MainTest::testRoundTripSerialization() {
    ProjectManager pm;
    pm.createNewProject("Test Project");
    
    auto sg = std::make_unique<SampleGroup>();
    sg->name = "Violin";
    sg->volume = -3.5;
    sg->pan = 0.5;
    sg->ampEnv.attack = 0.1;
    sg->ampEnv.release = 0.5;
    
    Zone z1;
    z1.samplePath = "violinc4.wav";
    z1.loNote = 60;
    z1.hiNote = 64;
    sg->zones.push_back(z1);
    
    QUuid id = sg->id;
    pm.addNode(std::move(sg));
    
    QString jsonString = ProjectSerializer::saveProject(&pm);
    QVERIFY(!jsonString.isEmpty());
    
    ProjectManager pmLoader;
    QVERIFY(ProjectSerializer::loadProject(&pmLoader, jsonString));
    pmLoader.setCurrentProjectName("Test Project"); // emulate what ProjectManager::loadProject does
    
    QCOMPARE(pmLoader.getCurrentProjectName(), QString("Test Project"));
    Node* loadedNode = pmLoader.getNode(id);
    QVERIFY(loadedNode != nullptr);
    
    SampleGroup* loadedSg = dynamic_cast<SampleGroup*>(loadedNode);
    QVERIFY(loadedSg != nullptr);
    QCOMPARE(loadedSg->name, QString("Violin"));
    QCOMPARE(loadedSg->volume, -3.5);
    QCOMPARE(loadedSg->pan, 0.5);
    QCOMPARE(loadedSg->ampEnv.attack, 0.1);
    QCOMPARE(loadedSg->ampEnv.release, 0.5);
    QCOMPARE(loadedSg->zones.size(), static_cast<size_t>(1));
    QCOMPARE(loadedSg->zones[0].samplePath, QString("violinc4.wav"));
    QCOMPARE(loadedSg->zones[0].loNote, 60);
}

void MainTest::testFilmstripMath() {
    FilmstripRenderer renderer;
    QVERIFY(!renderer.isValid());
    QCOMPARE(renderer.getFrameCount(), 1);
    
    QPixmap pixmap;
    QRect rect;
    QVERIFY(!renderer.getFrame(0.5, pixmap, rect));
}

void MainTest::testUiComponentSerialization() {
    ProjectManager pm;
    pm.createNewProject("UI Test");
    auto knob = std::make_unique<UiKnob>();
    knob->filmstripPath = "assets/knob.png";
    knob->frameCount = 101;
    knob->currentValue = 0.5;
    knob->name = "Cutoff";
    QUuid knobId = knob->id;
    
    pm.addUiComponent(std::move(knob));
    
    QString jsonString = ProjectSerializer::saveProject(&pm);
    QVERIFY(!jsonString.isEmpty());
    
    ProjectManager loadedPm;
    QVERIFY(ProjectSerializer::loadProject(&loadedPm, jsonString));
    
    UiComponent* loadedComp = loadedPm.getUiComponent(knobId);
    QVERIFY(loadedComp != nullptr);
    QCOMPARE(loadedComp->type, QString("UiKnob"));
    QCOMPARE(loadedComp->name, QString("Cutoff"));
    
    UiKnob* loadedKnob = static_cast<UiKnob*>(loadedComp);
    QCOMPARE(loadedKnob->filmstripPath, QString("assets/knob.png"));
    QCOMPARE(loadedKnob->frameCount, 101);
    QCOMPARE(loadedKnob->currentValue, 0.5);
}

void MainTest::testZoneCommands() {
    ProjectManager pm;
    auto sg = std::make_unique<SampleGroup>();
    QUuid sgId = sg->id;
    pm.getUndoStack()->push(new AddNodeCommand(&pm, std::move(sg), QPointF(0, 0)));
    
    Zone z1;
    z1.samplePath = "test1.wav";
    z1.loNote = 60;
    
    pm.getUndoStack()->push(new AddZoneCommand(&pm, sgId, z1));
    SampleGroup* node = static_cast<SampleGroup*>(pm.getNode(sgId));
    QCOMPARE(node->zones.size(), 1);
    QCOMPARE(node->zones[0].samplePath, QString("test1.wav"));
    
    pm.getUndoStack()->undo();
    QCOMPARE(node->zones.size(), 0);
    
    pm.getUndoStack()->redo();
    QCOMPARE(node->zones.size(), 1);
    
    Zone z2 = z1;
    z2.loNote = 64;
    pm.getUndoStack()->push(new ModifyZoneCommand(&pm, sgId, 0, z1, z2));
    QCOMPARE(node->zones[0].loNote, 64);
    
    pm.getUndoStack()->push(new RemoveZoneCommand(&pm, sgId, 0));
    QCOMPARE(node->zones.size(), 0);
    
    pm.getUndoStack()->undo();
    QCOMPARE(node->zones.size(), 1);
}

void MainTest::testWaveformCache() {
    WaveformCache cache;
    QSignalSpy spy(&cache, &WaveformCache::loadFinished);
    
    cache.loadAudio("non_existent_file.wav");
    spy.wait(1000); 
    
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toBool() == false);
    
    QPolygonF poly = cache.getPolygon(100, 100);
    QVERIFY(poly.isEmpty());
}

void MainTest::testDecentSamplerTranspiler() {
    ProjectManager pm;
    auto sg = std::make_unique<SampleGroup>();
    sg->name = "Piano";
    Zone z;
    z.samplePath = "piano.wav";
    z.loNote = 60;
    z.hiNote = 64;
    z.loVel = 0;
    z.hiVel = 127;
    sg->zones.push_back(z);
    QString sgId = sg->id.toString(); // capture before the node is moved into the project
    pm.addNode(std::move(sg));

    DecentSamplerTranspiler transpiler;
    QString xml = transpiler.generate(&pm, true);
    // Bundle export keeps the original extension (asFlac=false) — matches DsGroupBuilder.
    QString expectedPath = BundleExporter::getSafeExportName("piano.wav", false);

    // Attribute-order-independent assertions (DsDom serializes attributes alphabetically).
    // Root carries both the spec-correct minVersion and the legacy pluginVersion.
    QVERIFY(xml.contains("<DecentSampler"));
    QVERIFY(xml.contains("pluginVersion=\"1\""));
    QVERIFY(xml.contains("minVersion=\"1.10.0\""));
    // Group name gets the mic-layer suffix; the group is tagged by its UUID + mic name
    // so bindings/modulators can target it.
    QVERIFY(xml.contains("name=\"Piano_Close\""));
    QVERIFY(xml.contains(QString("tags=\"%1,Close\"").arg(sgId)));
    QVERIFY(xml.contains(QString("path=\"%1\"").arg(expectedPath)));
}

void MainTest::testSfzTranspiler() {
    ProjectManager pm;
    auto sg = std::make_unique<SampleGroup>();
    Zone z;
    z.samplePath = "bass.wav";
    z.loNote = 30;
    z.hiNote = 34;
    z.loVel = 0;
    z.hiVel = 127;
    sg->zones.push_back(z);
    pm.addNode(std::move(sg));
    
    SfzTranspiler transpiler;
    QString sfz = transpiler.generate(&pm);
    
    QVERIFY(sfz.contains("<group>"));
    QVERIFY(sfz.contains("<region>"));
    QVERIFY(sfz.contains("sample=bass.wav"));
    QVERIFY(sfz.contains("lokey=30 hikey=34"));
}

void MainTest::testFlacEncoderRoundTrip() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    struct Case { int bits; int channels; int frames; const char* name; };
    // 5000 frames spans two 4096-sample blocks, so the short trailing block is covered too.
    const Case cases[] = {
        { 16, 2, 5000, "stereo16.wav" },
        { 24, 1, 4096, "mono24.wav" },
        {  8, 1, 1000, "mono8.wav"   },
    };

    for (const Case& c : cases) {
        const std::vector<qint32> original = buildTestSignal(c.frames, c.channels, c.bits);
        const QByteArray wav = buildWav(original, c.channels, 44100, c.bits);

        const QString wavPath = dir.filePath(QString::fromLatin1(c.name));
        {
            QFile f(wavPath);
            QVERIFY(f.open(QIODevice::WriteOnly));
            QCOMPARE(static_cast<int>(f.write(wav)), wav.size());
        }

        QVERIFY2(FlacEncoder::canEncode(wavPath), c.name);

        QByteArray flac;
        QVERIFY2(FlacEncoder::encodeFile(wavPath, flac), c.name);
        QVERIFY2(flac.startsWith("fLaC"), c.name);

        // The whole point of the feature: the bundle entry must be smaller than the WAV.
        QVERIFY2(flac.size() < wav.size(), c.name);

        // And lossless: every sample must survive the round trip untouched.
        std::vector<qint32> decoded;
        QVERIFY2(decodeFlac(flac, c.bits, c.channels, decoded), c.name);
        QCOMPARE(decoded.size(), original.size());
        for (size_t i = 0; i < original.size(); ++i) {
            if (decoded[i] != original[i]) {
                QFAIL(qPrintable(QString("%1: sample %2 changed: %3 -> %4")
                                 .arg(c.name).arg(i).arg(original[i]).arg(decoded[i])));
            }
        }

        // The bundle name follows from the same decision the transpiler makes.
        QVERIFY(BundleExporter::getBundleSampleName(wavPath).endsWith(".flac"));
    }
}

void MainTest::testFlacEncoderRejectsFloatWav() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    // WAVE_FORMAT_IEEE_FLOAT cannot be encoded losslessly here, so it must be refused and
    // stored as-is rather than silently truncated to an integer depth.
    const QString floatPath = dir.filePath("float32.wav");
    {
        QByteArray wav = buildWav(std::vector<qint32>(256, 0), 1, 44100, 32, /*formatTag=*/3);
        QFile f(floatPath);
        QVERIFY(f.open(QIODevice::WriteOnly));
        QCOMPARE(static_cast<int>(f.write(wav)), wav.size());
    }

    QVERIFY(!FlacEncoder::canEncode(floatPath));
    QVERIFY(BundleExporter::getBundleSampleName(floatPath).endsWith(".wav"));

    // A path that is not a WAV at all must also be left alone.
    QVERIFY(!FlacEncoder::canEncode(dir.filePath("missing.wav")));
    QVERIFY(!FlacEncoder::canEncode(dir.filePath("sample.ogg")));
}

void MainTest::testBundleIncludesWavetable() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    // A real WAV, so the exporter has something to actually read and store.
    const QString wavetablePath = dir.filePath("table.wav");
    {
        const QByteArray wav = buildWav(buildTestSignal(512, 1, 16), 1, 44100, 16);
        QFile f(wavetablePath);
        QVERIFY(f.open(QIODevice::WriteOnly));
        QCOMPARE(static_cast<int>(f.write(wav)), wav.size());
    }

    ProjectManager pm;
    pm.createNewProject("WavetableBundle");

    auto sg = std::make_unique<SampleGroup>();
    sg->name = "Osc";
    sg->isOscillator = true;
    sg->oscParams.waveform = "wavetable";
    sg->oscParams.wavetableFile = wavetablePath;
    pm.addNode(std::move(sg));

    pm.getPresetManager()->addPreset("Main");

    const QString bundlePath = dir.filePath("out.dslibrary");
    QString error;
    QVERIFY2(BundleExporter::exportToDecentSampler(&pm, bundlePath, error), qPrintable(error));

    // The transpiler writes the wavetable under this name; the archive must contain it.
    const QString expected = BundleExporter::getSafeExportName(wavetablePath, false);

    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));
    QVERIFY(mz_zip_reader_init_file(&zip, bundlePath.toUtf8().constData(), 0));

    QStringList entries;
    const mz_uint count = mz_zip_reader_get_num_files(&zip);
    for (mz_uint i = 0; i < count; ++i) {
        mz_zip_archive_file_stat stat;
        if (mz_zip_reader_file_stat(&zip, i, &stat)) {
            entries << QString::fromUtf8(stat.m_filename);
        }
    }
    mz_zip_reader_end(&zip);

    QVERIFY2(entries.contains(expected),
             qPrintable(QString("wavetable '%1' missing from bundle; entries: %2")
                        .arg(expected, entries.join(", "))));
}


void MainTest::testDecibelConversion() {
    // 0 dB is unity. The whole model stores volumes in dB and defaults them to 0.0, so
    // getting this backwards renders every channel silent.
    QVERIFY(qFuzzyCompare(DecibelUtils::dbToLinear(0.0) + 1.0f, 2.0f));
    QVERIFY(DecibelUtils::dbToLinear(-6.0) < 0.51f);
    QVERIFY(DecibelUtils::dbToLinear(-6.0) > 0.49f);
    QVERIFY(DecibelUtils::dbToLinear(6.0) > 1.99f);
    QCOMPARE(DecibelUtils::dbToLinear(-96.0), 0.0f);
    QCOMPARE(DecibelUtils::dbToLinear(-200.0), 0.0f);

    // The fader speaks dB and must round-trip the values a user actually lands on.
    FaderWidget fader;
    for (double db : { 0.0, -6.0, 6.0, -30.0 }) {
        fader.setValueDb(db);
        QVERIFY2(std::abs(fader.valueDb() - db) < 0.5,
                 qPrintable(QString("fader lost %1 dB (got %2)").arg(db).arg(fader.valueDb())));
    }
    // A fader at unity must be exactly 0 dB, not merely close to it.
    fader.setValueDb(0.0);
    QVERIFY(std::abs(fader.valueDb()) < 1e-9);
}

void MainTest::testGroupVolumeExportedAsDecibels() {
    ProjectManager pm;

    auto sg = std::make_unique<SampleGroup>();
    sg->name = "Quiet";
    sg->volume = -6.0; // decibels
    Zone z;
    z.samplePath = "a.wav";
    sg->zones.push_back(z);
    pm.addNode(std::move(sg));

    auto loud = std::make_unique<SampleGroup>();
    loud->name = "Unity";
    loud->volume = 0.0; // 0 dB
    Zone z2;
    z2.samplePath = "b.wav";
    loud->zones.push_back(z2);
    pm.addNode(std::move(loud));

    DecentSamplerTranspiler transpiler;
    const QString xml = transpiler.generate(&pm);

    // A bare number means a linear 0..1 multiplier to Decent Sampler, so -6 would be a
    // 6x boost and 0 would be total silence. The suffix is what makes it decibels.
    QVERIFY2(xml.contains("volume=\"-6dB\""), qPrintable(xml));
    QVERIFY2(!xml.contains("volume=\"-6\""), qPrintable(xml));

    // 0 dB is unity, so the attribute is left out rather than written as a literal 0.
    QVERIFY2(!xml.contains("volume=\"0\""), qPrintable(xml));
}


void MainTest::testGroupInsertChainOrderExported() {
    // The mixer's FX rack writes insertEffects, and chain order changes the sound.
    // Connections cannot express order, which is why they are no longer the source.
    ProjectManager pm;

    auto delay = std::make_unique<DelayNode>();
    const QUuid delayId = delay->id;
    auto reverb = std::make_unique<ReverbNode>();
    const QUuid reverbId = reverb->id;
    pm.addNode(std::move(delay));
    pm.addNode(std::move(reverb));

    auto sg = std::make_unique<SampleGroup>();
    Zone z;
    z.samplePath = "a.wav";
    sg->zones.push_back(z);
    sg->insertEffects = { reverbId, delayId }; // reverb first, deliberately
    pm.addNode(std::move(sg));

    DecentSamplerTranspiler transpiler;
    const QString xml = transpiler.generate(&pm);

    const int reverbAt = xml.indexOf("\"reverb\"");
    const int delayAt = xml.indexOf("\"delay\"");
    QVERIFY2(reverbAt >= 0 && delayAt >= 0, qPrintable(xml));
    QVERIFY2(reverbAt < delayAt, "group insert chain was not exported in insertEffects order");
}

void MainTest::testEffectPositionSkipsBypassedAndCountsEqBands() {
    // Bindings address master effects by position. Two things break naive counting: a
    // bypassed node emits nothing, and an equalizer emits one <effect> per band.
    ProjectManager pm;

    auto bypassed = std::make_unique<DelayNode>();
    bypassed->bypassed = true;
    const QUuid bypassedId = bypassed->id;

    auto eq = std::make_unique<EqualizerNode>();
    const int bandCount = eq->bands.size();
    const QUuid eqId = eq->id;

    auto tail = std::make_unique<ChorusNode>();
    const QUuid tailId = tail->id;

    pm.addNode(std::move(bypassed));
    pm.addNode(std::move(eq));
    pm.addNode(std::move(tail));
    pm.getAudioState()->setMasterEffects({ bypassedId, eqId, tailId });

    QVERIFY(bandCount > 1); // otherwise the test proves nothing about band counting

    // The bypassed effect emits nothing, so it has no position at all.
    QCOMPARE(DsEffectBuilder::getEffectPosition(&pm, pm.getNode(bypassedId)), -1);

    // The equalizer starts at 0, since the bypassed node ahead of it emitted nothing.
    QCOMPARE(DsEffectBuilder::getEffectPosition(&pm, pm.getNode(eqId)), 0);

    // And the next effect sits after all of the equalizer's bands, not just after one.
    QCOMPARE(DsEffectBuilder::getEffectPosition(&pm, pm.getNode(tailId)), bandCount);
}


void MainTest::testBusRoutingPersists() {
    ProjectManager pm;

    auto bus = std::make_unique<BusNode>();
    bus->name = "Strings";
    const QUuid busId = bus->id;
    pm.addNode(std::move(bus));

    auto sg = std::make_unique<SampleGroup>();
    const QUuid sgId = sg->id;
    pm.addNode(std::move(sg));

    QVERIFY(pm.getOutputBus(sgId).isNull()); // unrouted channels feed master
    QVERIFY(pm.canRouteToBus(sgId, busId));

    static_cast<SampleGroup*>(pm.getNode(sgId))->outputBusId = busId;
    QCOMPARE(pm.getOutputBus(sgId), busId);

    // Survives a save/load round trip.
    const QJsonObject saved = ProjectSerializer::serializeState(&pm);
    ProjectManager reloaded;
    ProjectSerializer::deserializeState(&reloaded, saved);
    QCOMPARE(reloaded.getOutputBus(sgId), busId);
}

void MainTest::testBusRoutingRejectsCycles() {
    // A loop would stall the topological ordering the renderer depends on, so it has to
    // be refused at assignment rather than discovered later in the audio thread.
    ProjectManager pm;

    auto a = std::make_unique<BusNode>();
    const QUuid aId = a->id;
    auto b = std::make_unique<BusNode>();
    const QUuid bId = b->id;
    pm.addNode(std::move(a));
    pm.addNode(std::move(b));

    QVERIFY(!pm.canRouteToBus(aId, aId));               // straight to itself

    QVERIFY(pm.canRouteToBus(aId, bId));
    static_cast<BusNode*>(pm.getNode(aId))->outputBusId = bId; // a -> b

    QVERIFY(!pm.canRouteToBus(bId, aId));               // b -> a would close the loop

    // A group may still feed either of them.
    auto sg = std::make_unique<SampleGroup>();
    const QUuid sgId = sg->id;
    pm.addNode(std::move(sg));
    QVERIFY(pm.canRouteToBus(sgId, aId));

    // Clearing a route is always allowed, and a non-bus destination is never valid.
    QVERIFY(pm.canRouteToBus(aId, QUuid()));
    QVERIFY(!pm.canRouteToBus(aId, sgId));
}

QTEST_MAIN(MainTest)
#include "main_test.moc"
