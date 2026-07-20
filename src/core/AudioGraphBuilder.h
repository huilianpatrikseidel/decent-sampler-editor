#pragma once
#include "core/ProjectManager.h"
#include "audio/AudioEngine.h"
#include <vector>
#include <string>
#include <QUuid>

struct RenderZone {
    int loNote, hiNote, loVel, hiVel;
    std::string sampleId;
    double attack, decay, sustain, release, volume;
    
    bool hasFilter = false;
    float filterCutoff = 20000.0f;
    float filterResonance = 0.0f;
    int filterType = 0; // 0=LP, 1=HP, 2=BP, 3=Notch
    
    bool legatoEnabled = false;
    float glideTime = 0.0f;
    QUuid groupId;
    int paramBlockIndex = -1;
    
    float modAttack = 0.0f;
    float modDecay = 0.0f;
    float modSustain = 1.0f;
    float modRelease = 0.0f;
    
    // Oscillator data
    bool isOscillator = false;
    int oscWaveform = 1; // 0=Sine, 1=Saw, 2=Square, 3=Triangle, 4=Noise, 5=Pluck
    float oscDamping = 0.5f;
    
    RenderRouting routings[8];
    int numRoutings = 0;
};

#include "CoreLibExport.h"

class CORELIB_EXPORT AudioGraphBuilder {
public:
    static std::vector<RenderZone> buildRenderGraph(ProjectManager* pm, AudioEngine* audioEngine);
};
