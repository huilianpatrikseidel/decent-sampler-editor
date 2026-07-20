#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <QUuid>
#include <QJsonObject>
#include <unordered_map>
#include "AudioNodes.h" // For LFO
#include "SequenceStructures.h"

enum class BgMode {
    Stretch,
    Center,
    Absolute
};

struct MidiBinding {
    int ccNumber = 1;
    QString type = "effect";
    int position = 0;
    QString parameter = "FX_FILTER_FREQUENCY";
    QString translation = "linear";
    double outputMin = 0.0;
    double outputMax = 1.0;
    
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["ccNumber"] = ccNumber;
        obj["type"] = type;
        obj["position"] = position;
        obj["parameter"] = parameter;
        obj["translation"] = translation;
        obj["outputMin"] = outputMin;
        obj["outputMax"] = outputMax;
        return obj;
    }
};

struct KeyboardColor {
    int loNote = 0;
    int hiNote = 127;
    QString color = "FF000000";
    
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["loNote"] = loNote;
        obj["hiNote"] = hiNote;
        obj["color"] = color;
        return obj;
    }
};

#include "../CoreLibExport.h"

class CORELIB_EXPORT UiStateModel : public QObject {
    Q_OBJECT
public:
    explicit UiStateModel(QObject* parent = nullptr);

    int getWidth() const { return m_width; }
    void setWidth(int w);

    int getHeight() const { return m_height; }
    void setHeight(int h);

    void clear();

    QString getBgImage() const { return m_bgImage; }
    void setBgImage(const QString& path);

    BgMode getBgMode() const { return m_bgMode; }
    void setBgMode(BgMode mode);

    void setViewport(double x, double y, double zoom);
    void getViewport(double& x, double& y, double& zoom) const;

signals:
    void canvasSizeChanged(int width, int height);
    void bgImageChanged(const QString& path);
    void bgModeChanged(BgMode mode);
    void viewportChanged(double x, double y, double zoom);
    void stateModified(); // Triggered on any change

private:
    int m_width = 812;
    int m_height = 375;
    QString m_bgImage;
    BgMode m_bgMode = BgMode::Stretch;
    double m_viewportX = 0.0;
    double m_viewportY = 0.0;
    double m_viewportZoom = 1.0;
};

class CORELIB_EXPORT AudioStateModel : public QObject {
    Q_OBJECT
public:
    explicit AudioStateModel(QObject* parent = nullptr);

    double getGlobalVolume() const { return m_globalVolume; }
    void setGlobalVolume(double v);

    double getGlobalPan() const { return m_globalPan; }
    void setGlobalPan(double p);

    void clear();

    double getGlobalTuning() const { return m_globalTuning; }
    void setGlobalTuning(double t);

    double getEditorBpm() const { return m_editorBpm; }
    void setEditorBpm(double bpm);

    const QVector<LFO>& getGlobalLfos() const { return m_globalLfos; }
    void setGlobalLfos(const QVector<LFO>& lfos);
    void addGlobalLfo(const LFO& lfo);
    void removeGlobalLfo(const QUuid& id);
    void updateGlobalLfo(const QUuid& id, const LFO& lfo);

    const QVector<QUuid>& getMasterEffects() const { return m_masterEffects; }
    void setMasterEffects(const QVector<QUuid>& fx);

    const QVector<MidiBinding>& getMidiBindings() const { return m_midiBindings; }
    void setMidiBindings(const QVector<MidiBinding>& bindings);

    const QVector<NoteSequence>& getNoteSequences() const { return m_noteSequences; }
    void setNoteSequences(const QVector<NoteSequence>& seqs);

    const std::unordered_map<QString, int>& getTagPolyphony() const { return m_tagPolyphony; }
    void setTagPolyphony(const std::unordered_map<QString, int>& tags);
    void setSingleTagPolyphony(const QString& tag, int polyphony);

    const QVector<KeyboardColor>& getKeyboardColors() const { return m_keyboardColors; }
    void setKeyboardColors(const QVector<KeyboardColor>& colors);

signals:
    void globalLfoChanged();
    void masterEffectsChanged();
    void audioStateModified(); // Triggered on any change

private:
    double m_globalVolume = 0.0;
    double m_globalPan = 0.0;
    double m_globalTuning = 0.0;
    
    QVector<LFO> m_globalLfos;
    QVector<QUuid> m_masterEffects;
    QVector<MidiBinding> m_midiBindings;
    QVector<NoteSequence> m_noteSequences;
    std::unordered_map<QString, int> m_tagPolyphony;
    QVector<KeyboardColor> m_keyboardColors;
    double m_editorBpm = 120.0;
};
