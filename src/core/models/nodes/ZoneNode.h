#pragma once
#include <QString>
#include <QJsonObject>
#include "Modulators.h"

// Represents an audio file mapping with advanced multi-mic/keyswitch params
struct Zone {
    QString samplePath;
    int loNote = 0;
    int hiNote = 127;
    int rootNote = 60;
    int loVel = 1;
    int hiVel = 127;
    int seqPosition = 1; // Round Robins
    int keySwitchNote = -1; // -1 if unused
    QString micLayer = "Close"; // Multi-Mic Mix
    
    double tuning = 0.0;
    double volume = 0.0;
    double pan = 0.0;
    
    // Bounds and Looping
    long long sampleStart = 0;
    long long sampleEnd = 0; // 0 means end of file
    bool loopEnabled = false;
    long long loopStart = 0;
    long long loopEnd = 0;
    long long loopCrossfade = 0;
    
    bool useLocalAmpEnv = false;
    ADSR ampEnv;
    
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["samplePath"] = samplePath;
        obj["loNote"] = loNote; obj["hiNote"] = hiNote;
        obj["rootNote"] = rootNote;
        obj["loVel"] = loVel; obj["hiVel"] = hiVel;
        obj["seqPosition"] = seqPosition;
        obj["keySwitchNote"] = keySwitchNote;
        obj["micLayer"] = micLayer;
        obj["tuning"] = tuning;
        obj["volume"] = volume;
        obj["pan"] = pan;
        obj["loopEnabled"] = loopEnabled;
        obj["loopStart"] = loopStart;
        obj["loopEnd"] = loopEnd;
        obj["loopCrossfade"] = loopCrossfade;
        
        obj["useLocalAmpEnv"] = useLocalAmpEnv;
        if (useLocalAmpEnv) obj["ampEnv"] = ampEnv.toJson();
        
        obj["sampleStart"] = sampleStart;
        obj["sampleEnd"] = sampleEnd;
        
        return obj;
    }
};
