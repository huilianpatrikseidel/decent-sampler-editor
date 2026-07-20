#include "AudioStateSerializer.h"
#include "../models/ProjectStates.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QUuid>

QJsonObject AudioStateSerializer::serialize(const AudioStateModel* audioState) {
    QJsonObject state;
    
    state["globalVolume"] = audioState->getGlobalVolume();
    state["globalPan"] = audioState->getGlobalPan();
    state["globalTuning"] = audioState->getGlobalTuning();
    
    QJsonArray globalLfosArray;
    for (const auto& lfo : audioState->getGlobalLfos()) {
        globalLfosArray.append(lfo.toJson());
    }
    state["globalLfos"] = globalLfosArray;
    
    QJsonArray masterEffectsArray;
    for (const auto& fx : audioState->getMasterEffects()) {
        masterEffectsArray.append(fx.toString());
    }
    state["masterEffects"] = masterEffectsArray;
    
    QJsonArray midiArray;
    for (const auto& mb : audioState->getMidiBindings()) {
        midiArray.append(mb.toJson());
    }
    state["midiBindings"] = midiArray;
    
    QJsonArray seqArray;
    for (const auto& seq : audioState->getNoteSequences()) {
        seqArray.append(seq.toJson());
    }
    state["noteSequences"] = seqArray;
    
    QJsonObject tagPolyObj;
    for (const auto& pair : audioState->getTagPolyphony()) {
        tagPolyObj[pair.first] = pair.second;
    }
    state["tagPolyphony"] = tagPolyObj;
    
    QJsonArray kbColorsArray;
    for (const auto& kc : audioState->getKeyboardColors()) {
        kbColorsArray.append(kc.toJson());
    }
    state["keyboardColors"] = kbColorsArray;
    
    return state;
}

void AudioStateSerializer::deserialize(AudioStateModel* audioState, const QJsonObject& json) {
    if (json.contains("globalVolume")) audioState->setGlobalVolume(json["globalVolume"].toDouble());
    if (json.contains("globalPan")) audioState->setGlobalPan(json["globalPan"].toDouble());
    if (json.contains("globalTuning")) audioState->setGlobalTuning(json["globalTuning"].toDouble());
    
    if (json.contains("globalLfos")) {
        QJsonArray lfoArr = json["globalLfos"].toArray();
        QVector<LFO> lfos;
        for (int i = 0; i < lfoArr.size(); ++i) {
            LFO lfo;
            lfo.fromJson(lfoArr[i].toObject());
            lfos.push_back(lfo);
        }
        audioState->setGlobalLfos(lfos);
    }
    
    if (json.contains("masterEffects")) {
        QJsonArray fxArr = json["masterEffects"].toArray();
        QVector<QUuid> fxs;
        for (int i = 0; i < fxArr.size(); ++i) {
            fxs.push_back(QUuid(fxArr[i].toString()));
        }
        audioState->setMasterEffects(fxs);
    }
    
    if (json.contains("midiBindings")) {
        QJsonArray arr = json["midiBindings"].toArray();
        QVector<MidiBinding> binds;
        for (int i = 0; i < arr.size(); ++i) {
            QJsonObject obj = arr[i].toObject();
            MidiBinding b;
            b.ccNumber = obj["ccNumber"].toInt();
            b.type = obj["type"].toString();
            b.position = obj["position"].toInt();
            b.parameter = obj["parameter"].toString();
            b.translation = obj["translation"].toString();
            b.outputMin = obj["outputMin"].toDouble();
            b.outputMax = obj["outputMax"].toDouble();
            binds.push_back(b);
        }
        audioState->setMidiBindings(binds);
    }
    
    if (json.contains("noteSequences")) {
        QJsonArray seqArr = json["noteSequences"].toArray();
        QVector<NoteSequence> seqs;
        for (int i = 0; i < seqArr.size(); ++i) {
            NoteSequence seq;
            seq.fromJson(seqArr[i].toObject());
            seqs.push_back(seq);
        }
        audioState->setNoteSequences(seqs);
    }
    
    if (json.contains("tagPolyphony")) {
        QJsonObject tags = json["tagPolyphony"].toObject();
        std::unordered_map<QString, int> tagMap;
        for (auto it = tags.begin(); it != tags.end(); ++it) {
            tagMap[it.key()] = it.value().toInt();
        }
        audioState->setTagPolyphony(tagMap);
    }
    
    if (json.contains("keyboardColors")) {
        QJsonArray arr = json["keyboardColors"].toArray();
        QVector<KeyboardColor> colors;
        for (int i = 0; i < arr.size(); ++i) {
            QJsonObject obj = arr[i].toObject();
            KeyboardColor kc;
            kc.loNote = obj["loNote"].toInt();
            kc.hiNote = obj["hiNote"].toInt();
            kc.color = obj["color"].toString();
            colors.push_back(kc);
        }
        audioState->setKeyboardColors(colors);
    }
}
