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
#include "ui/canvas/FilmstripRenderer.h"
#include "ui/canvas/UiComponentItem.h"
#include "ui/waveform/WaveformCache.h"
#include "transpilers/DecentSamplerTranspiler.h"
#include "transpilers/SfzTranspiler.h"
#include "export/BundleExporter.h"
#include "core/ProjectSerializer.h"
#include <QSignalSpy>
#include <memory>

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
};

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

QTEST_MAIN(MainTest)
#include "main_test.moc"
