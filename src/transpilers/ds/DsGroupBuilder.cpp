#include "DsGroupBuilder.h"
#include "DsEffectBuilder.h"
#include "../../export/BundleExporter.h"
#include "../../core/INodeVisitor.h"

void DsGroupBuilder::buildGroups(DsNode* rootGroups, const ProjectManager* pm, bool isBundle) {
    class GroupsVisitor : public INodeVisitor {
        DsNode* rootGroups;
        const ProjectManager* pm;
        bool isBundle;
    public:
        GroupsVisitor(DsNode* r, const ProjectManager* p, bool b) 
            : rootGroups(r), pm(p), isBundle(b) {}
            
        void visit(const SampleGroup* sg) override {
            if (sg->isSynthContainer) return; // Do not export UI container
            
            QStringList micLayers;
            for (const Zone& z : sg->zones) {
                if (!micLayers.contains(z.micLayer)) micLayers.append(z.micLayer);
            }
            if (micLayers.isEmpty()) micLayers.append("");
            
            // Resolve Parent parameters
            double effVolume = sg->volume;
            double effPan = sg->pan;
            double effTuning = sg->tuning;
            QString effTrigger = sg->trigger;
            QString effSeqMode = sg->seqMode;
            QString effSilencedByTags = sg->silencedByTags;
            QString effSilencingMode = sg->silencingMode;
            int effLoCC64 = sg->loCC64;
            int effHiCC64 = sg->hiCC64;
            bool effLegatoEnabled = sg->legatoEnabled;
            double effGlideTime = sg->glideTime;
            QVector<ModRouting> effRoutings = sg->routings;
            
            if (!sg->synthParentId.isNull()) {
                Node* pNode = pm->getNode(sg->synthParentId);
                if (pNode && pNode->type == "SampleGroup") {
                    SampleGroup* parentSg = static_cast<SampleGroup*>(pNode);
                    effVolume += parentSg->volume; // Mix child and parent volumes
                    effPan += parentSg->pan;
                    effTuning += parentSg->tuning;
                    effTrigger = parentSg->trigger;
                    effSeqMode = parentSg->seqMode;
                    effSilencedByTags = parentSg->silencedByTags;
                    effSilencingMode = parentSg->silencingMode;
                    effLoCC64 = parentSg->loCC64;
                    effHiCC64 = parentSg->hiCC64;
                    effLegatoEnabled = parentSg->legatoEnabled;
                    effGlideTime = parentSg->glideTime;
                    effRoutings.append(parentSg->routings);
                }
            }
            
            for (const QString& mic : micLayers) {
                DsNode* groupNode = rootGroups->addChild("group");
                QString groupName = sg->name.isEmpty() ? sg->id.toString() : sg->name;
                if (!mic.isEmpty()) groupName += "_" + mic;
                
                QStringList combinedTags;
                combinedTags.append(sg->id.toString());
                if (!mic.isEmpty()) combinedTags.append(mic);
                if (!sg->customTags.isEmpty()) {
                    for (const QString& t : sg->customTags.split(",")) {
                        QString trimmed = t.trimmed();
                        if (!trimmed.isEmpty() && !combinedTags.contains(trimmed)) {
                            combinedTags.append(trimmed);
                        }
                    }
                }
                
                if (!combinedTags.isEmpty()) groupNode->setAttribute("tags", combinedTags.join(","));
                
                groupNode->setAttribute("name", groupName);
                groupNode->setAttribute("volume", effVolume);
                groupNode->setAttribute("pan", effPan);
                groupNode->setAttribute("tuning", effTuning);
                groupNode->setAttribute("attack", sg->ampEnv.attack);
                groupNode->setAttribute("decay", sg->ampEnv.decay);
                groupNode->setAttribute("sustain", sg->ampEnv.sustain);
                groupNode->setAttribute("release", sg->ampEnv.release);
                
                if (effTrigger != "attack") groupNode->setAttribute("trigger", effTrigger);
                if (effSeqMode != "normal") {
                    if (effSeqMode == "random" || effSeqMode == "true_random") {
                        groupNode->setAttribute("seqMode", "round_robin");
                    } else {
                        groupNode->setAttribute("seqMode", effSeqMode);
                    }
                }
                if (!effSilencedByTags.isEmpty()) {
                    groupNode->setAttribute("silencedByTags", effSilencedByTags);
                    if (effSilencingMode != "fast") groupNode->setAttribute("silencingMode", effSilencingMode);
                }
                if (effLoCC64 > 0) groupNode->setAttribute("loCC64", effLoCC64);
                if (effHiCC64 < 127) groupNode->setAttribute("hiCC64", effHiCC64);
                
                if (effLegatoEnabled) {
                    groupNode->setAttribute("silencingMode", "fast");
                    groupNode->setAttribute("legato", "true");
                    groupNode->setAttribute("glideTime", effGlideTime);
                }
                
                bool hasRoundRobin = false;
                for (const Zone& z : sg->zones) {
                    if (z.micLayer == mic && z.seqPosition > 1) {
                        hasRoundRobin = true; break;
                    }
                }
                if (hasRoundRobin && sg->seqMode == "normal") groupNode->setAttribute("seqMode", "round_robin");
                
                for (const Zone& z : sg->zones) {
                    if (z.micLayer != mic) continue;
                    
                    DsNode* node = nullptr;
                    if (sg->isOscillator) {
                        node = groupNode->addChild("oscillator");
                        node->setAttribute("type", sg->oscParams.waveform);
                        if (sg->oscParams.waveform == "wavetable") {
                            QString exportPath = sg->oscParams.wavetableFile;
                            if (isBundle) exportPath = BundleExporter::getSafeExportName(exportPath, false);
                            node->setAttribute("wavetableFile", exportPath);
                            node->setAttribute("wavetablePosition", sg->oscParams.wavetablePosition);
                            node->setAttribute("wavetableFrameInterpolation", sg->oscParams.wavetableFrameInterpolation ? "true" : "false");
                        } else if (sg->oscParams.waveform == "pluck1") {
                            node->setAttribute("pluckType", sg->oscParams.pluckType);
                            node->setAttribute("damping", sg->oscParams.damping);
                        }
                    } else {
                        node = groupNode->addChild("sample");
                        QString exportPath = z.samplePath;
                        if (isBundle) exportPath = BundleExporter::getSafeExportName(z.samplePath, false);
                        node->setAttribute("path", exportPath);
                        if (z.sampleStart > 0) node->setAttribute("start", z.sampleStart);
                        if (z.sampleEnd > 0) node->setAttribute("end", z.sampleEnd);
                        if (z.loopEnabled) {
                            node->setAttribute("loopEnabled", "true");
                            node->setAttribute("loopStart", z.loopStart);
                            node->setAttribute("loopEnd", z.loopEnd);
                            if (z.loopCrossfade > 0) node->setAttribute("loopCrossfade", z.loopCrossfade);
                        }
                    }
                    
                    node->setAttribute("loNote", z.loNote);
                    node->setAttribute("hiNote", z.hiNote);
                    node->setAttribute("rootNote", z.rootNote);
                    node->setAttribute("loVel", z.loVel);
                    node->setAttribute("hiVel", z.hiVel);
                    node->setAttribute("seqPosition", z.seqPosition);
                    node->setAttribute("tuning", z.tuning);
                    node->setAttribute("volume", z.volume);
                    node->setAttribute("pan", z.pan);
                    if (z.keySwitchNote != -1) node->setAttribute("keySwitch", z.keySwitchNote);
                    
                    if (z.useLocalAmpEnv) {
                        node->setAttribute("attack", z.ampEnv.attack);
                        node->setAttribute("decay", z.ampEnv.decay);
                        node->setAttribute("sustain", z.ampEnv.sustain);
                        node->setAttribute("release", z.ampEnv.release);
                    }
                }
                
                if (sg->filterEnabled) {
                    DsNode* filterNode = groupNode->addChild("filter");
                    filterNode->setAttribute("type", sg->filterType.toLower());
                    filterNode->setAttribute("frequency", sg->filterCutoff);
                    filterNode->setAttribute("resonance", sg->filterResonance);
                    
                    if (sg->filterKeyTrack > 0.0) {
                        DsNode* ktNode = filterNode->addChild("binding");
                        ktNode->setAttribute("type", "amp");
                        ktNode->setAttribute("level", "group");
                        ktNode->setAttribute("source", "PITCH");
                        ktNode->setAttribute("parameter", "FREQUENCY");
                        ktNode->setAttribute("translation", "linear");
                        ktNode->setAttribute("translationOutputMin", 0.0);
                        // Keytrack amount maps 0.0-1.0 to maybe 0 to 4800 cents? 1.0 means full tracking.
                        // Actually, Decent Sampler pitch is often 100 per semitone, so 1200 per octave.
                        // We'll map 0.0-1.0 to 0-4800 (4 octaves).
                        ktNode->setAttribute("translationOutputMax", sg->filterKeyTrack * 4800.0);
                    }
                    
                    if (sg->filterEnvDepth != 0.0) {
                        DsNode* modNode = filterNode->addChild("binding");
                        modNode->setAttribute("type", "amp");
                        modNode->setAttribute("level", "group");
                        modNode->setAttribute("source", "ENV2");
                        modNode->setAttribute("parameter", "FREQUENCY");
                        modNode->setAttribute("translation", "linear");
                        modNode->setAttribute("translationOutputMin", 0.0);
                        // Env depth maps -1.0 to 1.0 to -4800 to 4800 cents.
                        modNode->setAttribute("translationOutputMax", sg->filterEnvDepth * 4800.0);
                    }
                }
                
                auto conns = pm->getConnectionsForNode(sg->id);
                QList<Node*> localEffects;
                for (const auto& c : conns) {
                    if (c.sourceId == sg->id) {
                        Node* target = pm->getNode(c.targetId);
                        if (target && (target->type == "Delay" || target->type == "Reverb" || target->type == "Filter" ||
                                       target->type == "Gain" || target->type == "Phaser" || target->type == "PitchShifter" ||
                                       target->type == "WaveFolder" || target->type == "WaveShaper" ||
                                       target->type == "StereoSimulator" || target->type == "BitCrusher")) {
                            localEffects.append(target);
                        }
                    }
                }
                if (!localEffects.isEmpty()) {
                    DsNode* effectsNode = groupNode->addChild("effects");
                    for (Node* fx : localEffects) {
                        DsEffectBuilder::buildSingleEffect(effectsNode, fx);
                    }
                }
                
                if (!effRoutings.isEmpty()) {
                    for (const ModRouting& r : effRoutings) {
                        if (r.source == "LFO1" || r.source == "LFO2" || r.source == "Env1" || r.source == "Env2" || r.source == "ModWheel") continue;
                        
                        DsNode* bindingNode = groupNode->addChild("binding");
                        bindingNode->setAttribute("type", "amp");
                        QString sourceAttr = (r.source == "Velocity") ? "VELOCITY" : (r.source == "Aftertouch") ? "AFTERTOUCH" : (r.source == "PitchBend") ? "PITCHBEND" : r.source.toUpper();
                        QString destAttr = (r.destination == "Pitch") ? "PITCH" : (r.destination == "Volume") ? "VOLUME" : (r.destination == "Pan") ? "PAN" : r.destination.toUpper();
                        
                        bindingNode->setAttribute("level", "group");
                        bindingNode->setAttribute("source", sourceAttr);
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
                        if (r.modBehavior != "add") {
                            bindingNode->setAttribute("modBehavior", r.modBehavior);
                        }
                    }
                    
                    // Modulations
                    bool hasMods = false;
                    for (const ModRouting& r : effRoutings) {
                        if (r.source.startsWith("MACRO") || r.source.startsWith("LFO") || r.source.startsWith("ENV")) {
                            hasMods = true;
                            break;
                        }
                    }
                    if (hasMods) {
                        DsNode* modParentNode = groupNode->addChild("modulation");
                        for (const ModRouting& r : effRoutings) {
                            if (r.source.startsWith("MACRO") || r.source.startsWith("LFO") || r.source.startsWith("ENV")) {
                                DsNode* modNode = modParentNode->addChild("mod");
                                modNode->setAttribute("source", r.source);
                                modNode->setAttribute("target", r.destination);
                                modNode->setAttribute("amount", r.amount);
                            }
                        }
                    }
                }
            }
        }
        void visit(const BusNode*) override {}
        void visit(const DelayNode*) override {}
        void visit(const ReverbNode*) override {}
        void visit(const FilterNode*) override {}
        void visit(const ChorusNode*) override {}
        void visit(const GainNode*) override {}
        void visit(const PhaserNode*) override {}
        void visit(const PitchShifterNode*) override {}
        void visit(const WaveFolderNode*) override {}
        void visit(const WaveShaperNode*) override {}
        void visit(const StereoSimulatorNode*) override {}
        void visit(const BitCrusherNode*) override {}
        void visit(const EqualizerNode*) override {}
    };
    
    GroupsVisitor visitor(rootGroups, pm, isBundle);
    for (const auto& pair : pm->getAllNodes()) {
        pair.second->accept(&visitor);
    }
}
