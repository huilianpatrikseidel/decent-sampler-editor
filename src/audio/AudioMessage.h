#pragma once
#include <string>
#include <QUuid>

enum class ModSource { LFO1, LFO2, Env1, Env2, Velocity, ModWheel, PitchBend, Aftertouch, None };
enum class ModDest { Pitch, Volume, Pan, Cutoff, Resonance, None };

struct RenderRouting {
    ModSource source = ModSource::None;
    ModDest dest = ModDest::None;
    float amount = 0.0f;
};

enum class AudioCommandType {
    PlayNote,
    StopNote,
    SetMasterVolume,
    SetLFO,
    RebuildGraph,
    Shutdown
};

struct AudioMessage {
    AudioCommandType type;
    float value = 0.0f;
    
    // Polyphony Data
    int note = 60;
    int paramBlockIndex = -1; // Index in GlobalAudioState
    
    // Sample Playback
    int velocity = 100;
    std::string sampleId;
    
    // Envelope Data
    float attack = 0.0f;
    float decay = 0.0f;
    float sustain = 1.0f;
    float release = 0.0f;
    float volume = 1.0f;
    
    // Mod Envelope Data (Env2)
    float modAttack = 0.0f;
    float modDecay = 0.0f;
    float modSustain = 1.0f;
    float modRelease = 0.0f;
    
    // Legato Data
    bool isLegato = false;
    float glideTime = 0.0f;
    QUuid groupId;
    
    // Filter Data
    bool hasFilter = false;
    float filterCutoff = 20000.0f;
    float filterResonance = 0.0f;
    int filterType = 0; // 0=LP, 1=HP, 2=BP, 3=Notch
    
    // Oscillator Data (when isOscillator == true, no sample playback)
    bool isOscillator = false;
    int oscWaveform = 1; // 0=Sine, 1=Saw, 2=Square, 3=Triangle, 4=Noise, 5=Pluck
    float oscDamping = 0.5f; // For Karplus-Strong
    
    // LFO Data (when type == SetLFO)
    int lfoIndex = 1;
    int lfoShape = 0; // 0=Sine, 1=Tri, 2=Sq, 3=Saw
    float lfoFreq = 1.0f;
    float lfoAmount = 1.0f; // Global depth multiplier
    
    // Modulation Routings
    RenderRouting routings[8];
    int numRoutings = 0;
};
