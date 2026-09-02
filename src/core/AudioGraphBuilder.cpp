#include "AudioGraphBuilder.h"
#include "DecibelUtils.h"
#include "MixerTopology.h"
#include "audio/AudioEngine.h"

std::vector<RenderZone> AudioGraphBuilder::buildRenderGraph(ProjectManager* pm, AudioEngine* audioEngine) {
    std::vector<RenderZone> newZones;

    // One topology per rebuild: VstPluginManager builds its chains from the same call, so
    // both sides agree on which index is which channel.
    const MixerTopology topology = MixerTopology::build(pm, AudioEngine::MAX_CHANNELS);
    
    for (const auto& pair : pm->getAllNodes()) {
        const auto& node = pair.second;
        if (node->type == "SampleGroup") {
            SampleGroup* sg = static_cast<SampleGroup*>(node.get());
            
            // Helper lambda to populate common fields in a RenderZone
            auto populateCommon = [&](RenderZone& rz) {
                rz.attack = sg->ampEnv.attack;
                rz.decay = sg->ampEnv.decay;
                rz.sustain = sg->ampEnv.sustain;
                rz.release = sg->ampEnv.release;
                // sg->volume is decibels; the voice multiplies this straight into
                // its output, so it has to be linear by the time it gets there.
                rz.volume = DecibelUtils::dbToLinear(sg->volume);
                
                rz.modAttack = sg->modEnv.attack;
                rz.modDecay = sg->modEnv.decay;
                rz.modSustain = sg->modEnv.sustain;
                rz.modRelease = sg->modEnv.release;
                
                rz.legatoEnabled = sg->legatoEnabled;
                rz.glideTime = sg->glideTime;
                rz.groupId = sg->id;
                
                rz.channelIndex = topology.indexFor(sg->id);
                rz.paramBlockIndex = audioEngine->getAudioState()->getOrAllocateBlock(sg->id);
                if (rz.paramBlockIndex != -1) {
                    audioEngine->getAudioState()->nodeParams[rz.paramBlockIndex].volume.store(DecibelUtils::dbToLinear(sg->volume), std::memory_order_relaxed);
                    // Add other continuous parameters here (cutoff, resonance, etc.)
                }
                
                // Find connected filter
                auto conns = pm->getConnectionsForNode(sg->id);
                for (const auto& c : conns) {
                    if (c.sourceId == sg->id) {
                        Node* target = pm->getNode(c.targetId);
                        if (target && target->type == "Filter") {
                            FilterNode* fn = static_cast<FilterNode*>(target);
                            rz.hasFilter = true;
                            rz.filterCutoff = fn->cutoff;
                            rz.filterResonance = fn->resonance;
                            
                            if (rz.paramBlockIndex != -1) {
                                audioEngine->getAudioState()->nodeParams[rz.paramBlockIndex].cutoff.store(fn->cutoff, std::memory_order_relaxed);
                                audioEngine->getAudioState()->nodeParams[rz.paramBlockIndex].resonance.store(fn->resonance, std::memory_order_relaxed);
                            }
                            
                            // Map filter type string to int
                            QString ft = fn->filterType.toLower();
                            if (ft == "lowpass") rz.filterType = 0;
                            else if (ft == "highpass") rz.filterType = 1;
                            else if (ft == "bandpass") rz.filterType = 2;
                            else if (ft == "notch") rz.filterType = 3;
                            break;
                        }
                    }
                }
                
                // Mod routings
                rz.numRoutings = std::min((int)sg->routings.size(), 8);
                for (int i = 0; i < rz.numRoutings; ++i) {
                    const auto& r = sg->routings[i];
                    if (r.source == "LFO1") rz.routings[i].source = ModSource::LFO1;
                    else if (r.source == "LFO2") rz.routings[i].source = ModSource::LFO2;
                    else if (r.source == "Env1") rz.routings[i].source = ModSource::Env1;
                    else if (r.source == "Env2") rz.routings[i].source = ModSource::Env2;
                    else if (r.source == "Velocity") rz.routings[i].source = ModSource::Velocity;
                    else if (r.source == "ModWheel") rz.routings[i].source = ModSource::ModWheel;
                    else if (r.source == "PitchBend") rz.routings[i].source = ModSource::PitchBend;
                    else if (r.source == "Aftertouch") rz.routings[i].source = ModSource::Aftertouch;
                    
                    if (r.destination == "Pitch") rz.routings[i].dest = ModDest::Pitch;
                    else if (r.destination == "Volume") rz.routings[i].dest = ModDest::Volume;
                    else if (r.destination == "Pan") rz.routings[i].dest = ModDest::Pan;
                    else if (r.destination == "Cutoff") rz.routings[i].dest = ModDest::Cutoff;
                    else if (r.destination == "Resonance") rz.routings[i].dest = ModDest::Resonance;
                    
                    rz.routings[i].amount = r.amount;
                }
            };
            
            if (sg->isOscillator) {
                // Oscillator mode: create a single RenderZone covering full MIDI range
                RenderZone rz;
                rz.loNote = 0;
                rz.hiNote = 127;
                rz.loVel = 1;
                rz.hiVel = 127;
                rz.isOscillator = true;
                
                // Map waveform string to int
                QString wf = sg->oscParams.waveform.toLower();
                if (wf == "sine") rz.oscWaveform = 0;
                else if (wf == "saw" || wf == "sawtooth") rz.oscWaveform = 1;
                else if (wf == "square") rz.oscWaveform = 2;
                else if (wf == "triangle") rz.oscWaveform = 3;
                else if (wf == "noise") rz.oscWaveform = 4;
                else if (wf == "pluck") rz.oscWaveform = 5;
                else rz.oscWaveform = 1; // Default saw
                
                rz.oscDamping = sg->oscParams.damping;
                populateCommon(rz);
                newZones.push_back(rz);
            } else {
                // Sample playback mode: one RenderZone per Zone
                for (const Zone& z : sg->zones) {
                    RenderZone rz;
                    rz.loNote = z.loNote;
                    rz.hiNote = z.hiNote;
                    rz.loVel = z.loVel;
                    rz.hiVel = z.hiVel;
                    rz.sampleId = z.samplePath.toStdString();
                    populateCommon(rz);
                    newZones.push_back(rz);
                }
            }
        }
    }
    
    return newZones;
}
