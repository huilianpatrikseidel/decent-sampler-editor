#include "DecentSamplerTranspiler.h"
#include "ds/DsGroupBuilder.h"
#include "ds/DsEffectBuilder.h"
#include "ds/DsUiBuilder.h"

QString DecentSamplerTranspiler::generate(const ProjectManager* pm, bool isBundle, const QString& sampleDirPrefix, const QString& bgImageName) {
    auto audio = pm->getAudioState();
    
    DsDocument doc;
    doc.setAttribute("minVersion", "1.10.0");
    DsNode* uiNode = doc.addChild("ui");
    DsUiBuilder::buildUi(uiNode, pm, bgImageName);
    
    DsNode* groupsNode = doc.addChild("groups");
    if (audio->getGlobalVolume() != 0.0) groupsNode->setAttribute("volume", QString::number(audio->getGlobalVolume()) + "dB");
    if (audio->getGlobalPan() != 0.0) groupsNode->setAttribute("pan", audio->getGlobalPan());
    if (audio->getGlobalTuning() != 0.0) groupsNode->setAttribute("tuning", audio->getGlobalTuning());
    
    // MISS-10: Polyphony per tag
    if (!audio->getTagPolyphony().empty()) {
        DsNode* tagsNode = groupsNode->addChild("tags");
        for (const auto& pair : audio->getTagPolyphony()) {
            DsNode* t = tagsNode->addChild("tag");
            t->setAttribute("name", pair.first);
            t->setAttribute("polyphony", pair.second);
        }
    }
    
    DsGroupBuilder::buildGroups(groupsNode, pm, isBundle);
    
    if (!audio->getMidiBindings().isEmpty()) {
        DsNode* midiNode = doc.addChild("midi");
        for (const auto& mb : audio->getMidiBindings()) {
            DsNode* ccNode = midiNode->addChild("cc");
            ccNode->setAttribute("number", mb.ccNumber);
            DsNode* bindingNode = ccNode->addChild("binding");
            bindingNode->setAttribute("type", mb.type);
            if (mb.type == "effect") bindingNode->setAttribute("position", mb.position);
            bindingNode->setAttribute("parameter", mb.parameter);
            bindingNode->setAttribute("translation", mb.translation);
            if (mb.translation == "linear") {
                bindingNode->setAttribute("translationOutputMin", mb.outputMin);
                bindingNode->setAttribute("translationOutputMax", mb.outputMax);
            } else if (mb.translation == "table") {
                // Not supported yet in MidiBinding, but just preparing
            } else if (mb.translation == "fixed_value") {
                // Not supported yet in MidiBinding, but just preparing
            }
        }
    }
    
    // MISS-04: Note Sequences
    if (!audio->getNoteSequences().isEmpty()) {
        DsNode* nsNode = doc.addChild("noteSequences");
        for (const auto& seq : audio->getNoteSequences()) {
            DsNode* seqNode = nsNode->addChild("sequence");
            seqNode->setAttribute("name", seq.name);
            seqNode->setAttribute("length", seq.length);
            seqNode->setAttribute("rate", seq.rate);
            
            for (const auto& step : seq.steps) {
                DsNode* stepNode = seqNode->addChild("note");
                stepNode->setAttribute("position", step.position);
                stepNode->setAttribute("velocity", step.velocity);
                stepNode->setAttribute("note", step.note);
                stepNode->setAttribute("length", step.length);
            }
        }
    }
    
    DsNode* effectsNode = doc.addChild("effects");
    DsEffectBuilder::buildEffects(effectsNode, pm);
    
    DsNode* modulatorsNode = doc.addChild("modulators");
    
    auto addLfo = [&](const QString& name, LFO data) {
        DsNode* lfoNode = modulatorsNode->addChild("lfo");
        lfoNode->setAttribute("name", name);
        QString shape = data.shape.toLower();
        if (shape == "triangle") shape = "saw";
        lfoNode->setAttribute("shape", shape);
        lfoNode->setAttribute("frequency", data.frequency);
        lfoNode->setAttribute("modAmount", data.amount);
        lfoNode->setAttribute("scope", data.scope);
        lfoNode->setAttribute("delayTime", data.delayTime);
        
        for (const auto& pair : pm->getAllNodes()) {
            if (pair.second->type == "SampleGroup") {
                SampleGroup* sg = static_cast<SampleGroup*>(pair.second.get());
                for (const ModRouting& r : sg->routings) {
                    if (r.source == name) {
                        DsNode* bindingNode = lfoNode->addChild("binding");
                        bindingNode->setAttribute("type", "amp");
                        bindingNode->setAttribute("level", "group");
                        bindingNode->setAttribute("tags", sg->id.toString());
                        
                        QString destAttr = (r.destination == "Pitch") ? "PITCH" : (r.destination == "Volume") ? "VOLUME" : (r.destination == "Pan") ? "PAN" : r.destination.toUpper();
                        bindingNode->setAttribute("parameter", destAttr);
                        bindingNode->setAttribute("translation", r.translation);
                        if (r.translation == "linear") {
                            bindingNode->setAttribute("translationOutputMin", 0.0);
                            bindingNode->setAttribute("translationOutputMax", r.amount);
                        } else if (r.translation == "table") {
                            bindingNode->setAttribute("translationTable", r.translationTable);
                        } else if (r.translation == "fixed_value") {
                            bindingNode->setAttribute("translationValue", r.translationValue);
                        }
                        if (r.modBehavior != "add") bindingNode->setAttribute("modBehavior", r.modBehavior);
                    }
                }
            }
        }
    };
    
    auto lfos = audio->getGlobalLfos();
    for (const auto& lfo : lfos) {
        addLfo(lfo.name, lfo);
    }
    
    for (const auto& pair : pm->getAllNodes()) {
        if (pair.second->type == "SampleGroup") {
            SampleGroup* sg = static_cast<SampleGroup*>(pair.second.get());
            
            bool hasEnvMod = false;
            for (const auto& r : sg->routings) {
                if (r.source == "Env1" || r.source == "Env2") hasEnvMod = true;
            }
            if (hasEnvMod) {
                DsNode* envNode = modulatorsNode->addChild("envelope");
                envNode->setAttribute("attack", sg->modEnv.attack);
                envNode->setAttribute("decay", sg->modEnv.decay);
                envNode->setAttribute("sustain", sg->modEnv.sustain);
                envNode->setAttribute("release", sg->modEnv.release);
                
                for (const auto& r : sg->routings) {
                    if (r.source == "Env1" || r.source == "Env2") {
                        DsNode* bind = envNode->addChild("binding");
                        bind->setAttribute("type", "amp");
                        bind->setAttribute("level", "group");
                        bind->setAttribute("tags", sg->id.toString());
                        QString dest = (r.destination == "Pitch") ? "PITCH" : (r.destination == "Volume") ? "VOLUME" : (r.destination == "Pan") ? "PAN" : r.destination.toUpper();
                        bind->setAttribute("parameter", dest);
                        bind->setAttribute("translation", r.translation);
                        if (r.translation == "linear") {
                            bind->setAttribute("translationOutputMin", 0.0);
                            bind->setAttribute("translationOutputMax", r.amount);
                        } else if (r.translation == "table") {
                            bind->setAttribute("translationTable", r.translationTable);
                        } else if (r.translation == "fixed_value") {
                            bind->setAttribute("translationValue", r.translationValue);
                        }
                        if (r.modBehavior != "add") bind->setAttribute("modBehavior", r.modBehavior);
                    }
                }
            }
            
            for (const auto& r : sg->routings) {
                if (r.source == "ModWheel") { // Apenas CCs genuínos
                    DsNode* ccNode = modulatorsNode->addChild("midiCC");
                    ccNode->setAttribute("scope", "voice");
                    ccNode->setAttribute("number", 1);
                    ccNode->setAttribute("modAmount", 1.0);
                    
                    DsNode* bind = ccNode->addChild("binding");
                    bind->setAttribute("type", "amp");
                    bind->setAttribute("level", "group");
                    bind->setAttribute("tags", sg->id.toString());
                    QString dest = (r.destination == "Pitch") ? "PITCH" : (r.destination == "Volume") ? "VOLUME" : (r.destination == "Pan") ? "PAN" : r.destination.toUpper();
                    bind->setAttribute("parameter", dest);
                    bind->setAttribute("translation", r.translation);
                    if (r.translation == "linear") {
                        bind->setAttribute("translationOutputMin", 0.0);
                        bind->setAttribute("translationOutputMax", r.amount);
                    } else if (r.translation == "table") {
                        bind->setAttribute("translationTable", r.translationTable);
                    } else if (r.translation == "fixed_value") {
                        bind->setAttribute("translationValue", r.translationValue);
                    }
                    if (r.modBehavior != "add") bind->setAttribute("modBehavior", r.modBehavior);
                }
            }
        }
    }
    
    return doc.toXmlString();
}
