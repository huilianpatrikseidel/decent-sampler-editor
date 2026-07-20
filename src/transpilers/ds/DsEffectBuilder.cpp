#include "DsEffectBuilder.h"
#include "../../core/INodeVisitor.h"

int DsEffectBuilder::getEffectPosition(const ProjectManager* pm, const Node* effectNode) {
    bool isGlobal = true;
    for (const auto& c : pm->getConnectionsForNode(effectNode->id)) {
        if (c.targetId == effectNode->id) {
            Node* src = pm->getNode(c.sourceId);
            if (src && src->type == "SampleGroup") {
                isGlobal = false;
                break;
            }
        }
    }
    
    if (isGlobal) {
        int pos = 0;
        for (const auto& pair : pm->getAllNodes()) {
            const auto& node = pair.second;
            if (node->type == "Delay" || node->type == "Reverb" || node->type == "Filter" ||
                node->type == "Gain" || node->type == "Phaser" || node->type == "PitchShifter" ||
                node->type == "WaveFolder" || node->type == "WaveShaper" ||
                node->type == "StereoSimulator" || node->type == "BitCrusher") {
                bool g = true;
                for (const auto& c : pm->getConnectionsForNode(node->id)) {
                    if (c.targetId == node->id) {
                        Node* src = pm->getNode(c.sourceId);
                        if (src && src->type == "SampleGroup") {
                            g = false; break;
                        }
                    }
                }
                if (node->id == effectNode->id) return pos;
                if (g) pos++;
            }
        }
    }
    return -1;
}

void DsEffectBuilder::buildSingleEffect(DsNode* parent, const Node* node) {
    class SingleEffectVisitor : public INodeVisitor {
        DsNode* parent;
    public:
        SingleEffectVisitor(DsNode* p) : parent(p) {}
        void visit(const SampleGroup*) override {}
        void visit(const BusNode*) override {}
        void visit(const DelayNode* d) override {
            if (d->bypassed) return;
            DsNode* effectNode = parent->addChild("effect");
            if (!d->tags.isEmpty()) effectNode->setAttribute("tags", d->tags);
            effectNode->setAttribute("type", "delay");
            effectNode->setAttribute("delayTime", d->time);
            effectNode->setAttribute("feedback", d->feedback);
            effectNode->setAttribute("wetLevel", d->wetLevel);
            effectNode->setAttribute("stereoOffset", d->stereoOffset);
        }
        void visit(const ReverbNode* r) override {
            if (r->bypassed) return;
            DsNode* effectNode = parent->addChild("effect");
            if (!r->tags.isEmpty()) effectNode->setAttribute("tags", r->tags);
            if (!r->irPath.isEmpty()) {
                effectNode->setAttribute("type", "convolution");
                effectNode->setAttribute("irFile", r->irPath);
                effectNode->setAttribute("mix", r->wetLevel);
            } else {
                effectNode->setAttribute("type", "reverb");
                effectNode->setAttribute("roomSize", r->size);
                effectNode->setAttribute("damping", r->damp);
                effectNode->setAttribute("wetLevel", r->wetLevel);
            }
        }
        void visit(const FilterNode* filter) override {
            if (filter->bypassed) return;
            DsNode* fx = parent->addChild("effect");
            if (!filter->tags.isEmpty()) fx->setAttribute("tags", filter->tags);
            QString dsFilterType = filter->filterType.toLower();
            fx->setAttribute("type", dsFilterType);
            fx->setAttribute("frequency", QString::number(filter->cutoff));
            fx->setAttribute("resonance", QString::number(filter->resonance));
        }
        
        void visit(const ChorusNode* chorus) override {
            if (chorus->bypassed) return;
            DsNode* fx = parent->addChild("effect");
            if (!chorus->tags.isEmpty()) fx->setAttribute("tags", chorus->tags);
            fx->setAttribute("type", "chorus");
            fx->setAttribute("modRate", QString::number(chorus->rate));
            fx->setAttribute("modDepth", QString::number(chorus->depth));
            fx->setAttribute("mix", QString::number(chorus->mix));
        }
        

        
        void visit(const GainNode* g) override {
            if (g->bypassed) return;
            DsNode* fx = parent->addChild("effect");
            if (!g->tags.isEmpty()) fx->setAttribute("tags", g->tags);
            fx->setAttribute("type", "gain");
            fx->setAttribute("levelUnit", g->levelUnit);
            fx->setAttribute("level", QString::number(g->level));
        }
        
        void visit(const PhaserNode* p) override {
            if (p->bypassed) return;
            DsNode* fx = parent->addChild("effect");
            if (!p->tags.isEmpty()) fx->setAttribute("tags", p->tags);
            fx->setAttribute("type", "phaser");
            fx->setAttribute("mix", QString::number(p->mix));
            fx->setAttribute("modDepth", QString::number(p->modDepth));
            fx->setAttribute("modRate", QString::number(p->modRate));
            fx->setAttribute("centerFrequency", QString::number(p->centerFrequency));
            fx->setAttribute("feedback", QString::number(p->feedback));
        }
        
        void visit(const PitchShifterNode* p) override {
            if (p->bypassed) return;
            DsNode* fx = parent->addChild("effect");
            if (!p->tags.isEmpty()) fx->setAttribute("tags", p->tags);
            fx->setAttribute("type", "pitch_shift");
            fx->setAttribute("pitchShift", QString::number(p->pitchShift));
            fx->setAttribute("mix", QString::number(p->mix));
        }
        
        void visit(const WaveFolderNode* w) override {
            if (w->bypassed) return;
            DsNode* fx = parent->addChild("effect");
            if (!w->tags.isEmpty()) fx->setAttribute("tags", w->tags);
            fx->setAttribute("type", "wave_folder");
            fx->setAttribute("drive", QString::number(w->drive));
            fx->setAttribute("threshold", QString::number(w->threshold));
        }
        
        void visit(const WaveShaperNode* w) override {
            if (w->bypassed) return;
            DsNode* fx = parent->addChild("effect");
            if (!w->tags.isEmpty()) fx->setAttribute("tags", w->tags);
            fx->setAttribute("type", "wave_shaper");
            fx->setAttribute("drive", QString::number(w->drive));
            fx->setAttribute("driveBoost", QString::number(w->driveBoost));
            fx->setAttribute("outputLevel", QString::number(w->outputLevel));
            fx->setAttribute("highQuality", w->highQuality ? "true" : "false");
        }
        
        void visit(const StereoSimulatorNode* s) override {
            if (s->bypassed) return;
            DsNode* fx = parent->addChild("effect");
            if (!s->tags.isEmpty()) fx->setAttribute("tags", s->tags);
            fx->setAttribute("type", "stereo_simulator");
            fx->setAttribute("algorithm", s->algorithm);
            fx->setAttribute("width", QString::number(s->width));
            fx->setAttribute("delayTime", QString::number(s->delayTime));
            fx->setAttribute("modRate", QString::number(s->modRate));
            fx->setAttribute("modDepth", QString::number(s->modDepth));
        }
        
        void visit(const BitCrusherNode* b) override {
            if (b->bypassed) return;
            DsNode* fx = parent->addChild("effect");
            if (!b->tags.isEmpty()) fx->setAttribute("tags", b->tags);
            fx->setAttribute("type", "bit_crusher");
            fx->setAttribute("bitDepth", QString::number(b->bitDepth));
            fx->setAttribute("sampleRateReduction", QString::number(b->sampleRateReduction));
            fx->setAttribute("mix", QString::number(b->mix));
        }
        
        void visit(const EqualizerNode* eq) override {
            if (eq->bypassed) return;
            for (const auto& band : eq->bands) {
                DsNode* fx = parent->addChild("effect");
                if (!eq->tags.isEmpty()) fx->setAttribute("tags", eq->tags);
                QString dsFilterType = band.type.toLower();
                fx->setAttribute("type", dsFilterType);
                fx->setAttribute("frequency", QString::number(band.freq));
                
                // Decent Sampler doesn't natively map 'gain' strictly in all filter types except maybe peak/shelves, 
                // but we output it anyway so the engine can parse it if supported or script can use it.
                if (band.type == "Peak" || band.type == "LowShelf" || band.type == "HighShelf") {
                    fx->setAttribute("gain", QString::number(band.gain));
                }
                fx->setAttribute("resonance", QString::number(band.q));
            }
        }
    };
    SingleEffectVisitor visitor(parent);
    node->accept(&visitor);
}

void DsEffectBuilder::buildEffects(DsNode* rootEffects, const ProjectManager* pm) {
    for (const auto& pair : pm->getAllNodes()) {
        const auto& node = pair.second;
        if (node->type == "Delay" || node->type == "Reverb" || node->type == "Filter" ||
            node->type == "Gain" || node->type == "Phaser" || node->type == "PitchShifter" ||
            node->type == "WaveFolder" || node->type == "WaveShaper" ||
            node->type == "StereoSimulator" || node->type == "BitCrusher") {
            bool isGlobal = true;
            for (const auto& c : pm->getConnectionsForNode(node->id)) {
                if (c.targetId == node->id) {
                    Node* src = pm->getNode(c.sourceId);
                    if (src && src->type == "SampleGroup") {
                        isGlobal = false;
                        break;
                    }
                }
            }
            if (isGlobal) {
                buildSingleEffect(rootEffects, node.get());
            }
        }
    }
}
