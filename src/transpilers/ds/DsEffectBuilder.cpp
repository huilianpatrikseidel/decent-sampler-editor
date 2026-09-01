#include "DsEffectBuilder.h"
#include "../../core/INodeVisitor.h"

// Instrument-level effects, in the order they are emitted. Master is the only chain that
// lives at the top level, so its list defines both what is emitted and the positional
// index that UI bindings use to address an effect.
static QVector<QUuid> masterChain(const ProjectManager* pm) {
    return pm->getAudioState()->getMasterEffects();
}

// How many <effect> elements a node actually emits. Two cases stop this from being 1:
// a bypassed node emits nothing, and an equalizer emits one element per band. Bindings
// address effects by position, so the count has to match the emission exactly or every
// binding after an equalizer points at the wrong effect.
static int emittedEffectCount(const Node* n) {
    if (!n || n->bypassed) return 0;
    if (n->type == "Equalizer") {
        return static_cast<const EqualizerNode*>(n)->bands.size();
    }
    return 1;
}

int DsEffectBuilder::getEffectPosition(const ProjectManager* pm, const Node* effectNode) {
    int pos = 0;
    for (const QUuid& id : masterChain(pm)) {
        Node* n = pm->getNode(id);
        if (!n) continue;
        if (n->id == effectNode->id) {
            return n->bypassed ? -1 : pos; // a bypassed effect has no position to bind to
        }
        pos += emittedEffectCount(n);
    }
    return -1; // not on the master chain: a group insert, addressed within its group
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
    // Master inserts only. Group inserts are emitted inside their own <group> element by
    // DsGroupBuilder, from that group's insertEffects list.
    for (const QUuid& id : masterChain(pm)) {
        if (Node* n = pm->getNode(id)) {
            buildSingleEffect(rootEffects, n);
        }
    }
}
