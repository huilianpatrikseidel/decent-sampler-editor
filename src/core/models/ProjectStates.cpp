#include "ProjectStates.h"

// ---------------------------------------------------------
// UiStateModel
// ---------------------------------------------------------
UiStateModel::UiStateModel(QObject* parent) : QObject(parent) {}

void UiStateModel::clear() {
    m_width = 812;
    m_height = 375;
    m_bgImage.clear();
    m_bgMode = BgMode::Stretch;
    m_viewportX = 0.0;
    m_viewportY = 0.0;
    m_viewportZoom = 1.0;
    emit canvasSizeChanged(m_width, m_height);
    emit bgImageChanged(m_bgImage);
    emit bgModeChanged(m_bgMode);
    emit viewportChanged(m_viewportX, m_viewportY, m_viewportZoom);
    emit stateModified();
}

void UiStateModel::setWidth(int w) {
    if (m_width != w) {
        m_width = w;
        emit canvasSizeChanged(m_width, m_height);
        emit stateModified();
    }
}

void UiStateModel::setHeight(int h) {
    if (m_height != h) {
        m_height = h;
        emit canvasSizeChanged(m_width, m_height);
        emit stateModified();
    }
}

void UiStateModel::setBgImage(const QString& path) {
    if (m_bgImage != path) {
        m_bgImage = path;
        emit bgImageChanged(path);
        emit stateModified();
    }
}

void UiStateModel::setBgMode(BgMode mode) {
    if (m_bgMode != mode) {
        m_bgMode = mode;
        emit bgModeChanged(mode);
        emit stateModified();
    }
}

void UiStateModel::setViewport(double x, double y, double zoom) {
    if (m_viewportX != x || m_viewportY != y || m_viewportZoom != zoom) {
        m_viewportX = x;
        m_viewportY = y;
        m_viewportZoom = zoom;
        emit viewportChanged(x, y, zoom);
        emit stateModified();
    }
}

void UiStateModel::getViewport(double& x, double& y, double& zoom) const {
    x = m_viewportX;
    y = m_viewportY;
    zoom = m_viewportZoom;
}


// ---------------------------------------------------------
// AudioStateModel
// ---------------------------------------------------------
AudioStateModel::AudioStateModel(QObject* parent) : QObject(parent) {}

void AudioStateModel::clear() {
    m_globalVolume = 0.0;
    m_globalPan = 0.0;
    m_globalTuning = 0.0;
    m_globalLfos.clear();
    m_masterEffects.clear();
    m_midiBindings.clear();
    m_noteSequences.clear();
    m_tagPolyphony.clear();
    m_keyboardColors.clear();
    m_editorBpm = 120.0;
    
    emit globalLfoChanged();
    emit masterEffectsChanged();
    emit audioStateModified();
}

void AudioStateModel::setGlobalVolume(double v) {
    if (m_globalVolume != v) {
        m_globalVolume = v;
        emit audioStateModified();
    }
}

void AudioStateModel::setGlobalPan(double p) {
    if (m_globalPan != p) {
        m_globalPan = p;
        emit audioStateModified();
    }
}

void AudioStateModel::setGlobalTuning(double t) {
    if (m_globalTuning != t) {
        m_globalTuning = t;
        emit audioStateModified();
    }
}

void AudioStateModel::setEditorBpm(double bpm) {
    if (m_editorBpm != bpm) {
        m_editorBpm = bpm;
        emit audioStateModified();
    }
}

void AudioStateModel::setGlobalLfos(const QVector<LFO>& lfos) {
    m_globalLfos = lfos;
    emit globalLfoChanged();
    emit audioStateModified();
}

void AudioStateModel::addGlobalLfo(const LFO& lfo) {
    m_globalLfos.append(lfo);
    emit globalLfoChanged();
    emit audioStateModified();
}

void AudioStateModel::removeGlobalLfo(const QUuid& id) {
    auto it = std::remove_if(m_globalLfos.begin(), m_globalLfos.end(), [&](const LFO& lfo) { return lfo.id == id; });
    if (it != m_globalLfos.end()) {
        m_globalLfos.erase(it, m_globalLfos.end());
        emit globalLfoChanged();
        emit audioStateModified();
    }
}

void AudioStateModel::updateGlobalLfo(const QUuid& id, const LFO& lfo) {
    for (int i = 0; i < m_globalLfos.size(); ++i) {
        if (m_globalLfos[i].id == id) {
            m_globalLfos[i] = lfo;
            emit globalLfoChanged();
            emit audioStateModified();
            return;
        }
    }
}

void AudioStateModel::setMasterEffects(const QVector<QUuid>& fx) {
    m_masterEffects = fx;
    emit masterEffectsChanged();
    emit audioStateModified();
}

void AudioStateModel::setMidiBindings(const QVector<MidiBinding>& bindings) {
    m_midiBindings = bindings;
    emit audioStateModified();
}

void AudioStateModel::setNoteSequences(const QVector<NoteSequence>& seqs) {
    m_noteSequences = seqs;
    emit audioStateModified();
}

void AudioStateModel::setTagPolyphony(const std::unordered_map<QString, int>& tags) {
    m_tagPolyphony = tags;
    emit audioStateModified();
}

void AudioStateModel::setSingleTagPolyphony(const QString& tag, int polyphony) {
    m_tagPolyphony[tag] = polyphony;
    emit audioStateModified();
}

void AudioStateModel::setKeyboardColors(const QVector<KeyboardColor>& colors) {
    m_keyboardColors = colors;
    emit audioStateModified();
}
