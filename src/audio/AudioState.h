#pragma once
#include <atomic>
#include <QUuid>

// Lock-free shared state for continuous parameters
struct NodeParamBlock {
    std::atomic<bool> active{false};
    QUuid nodeId;
    
    std::atomic<float> volume{1.0f};
    std::atomic<float> pan{0.0f};
    std::atomic<float> cutoff{20000.0f};
    std::atomic<float> resonance{0.1f};
    std::atomic<float> oscDamping{0.0f};
    std::atomic<float> pitchOffset{0.0f};
};

class GlobalAudioState {
public:
    static const int MAX_NODES = 256;
    
    std::atomic<float> masterVolume{1.0f};
    NodeParamBlock nodeParams[MAX_NODES];
    
    int getOrAllocateBlock(const QUuid& id) {
        if (id.isNull()) return -1;
        
        // Find existing
        for (int i = 0; i < MAX_NODES; ++i) {
            if (nodeParams[i].active.load(std::memory_order_acquire) && nodeParams[i].nodeId == id) {
                return i;
            }
        }
        // Allocate new (only UI thread should allocate)
        for (int i = 0; i < MAX_NODES; ++i) {
            bool expected = false;
            if (nodeParams[i].active.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
                nodeParams[i].nodeId = id;
                return i;
            }
        }
        return -1;
    }
    
    void freeBlock(const QUuid& id) {
        for (int i = 0; i < MAX_NODES; ++i) {
            if (nodeParams[i].active.load(std::memory_order_acquire) && nodeParams[i].nodeId == id) {
                nodeParams[i].active.store(false, std::memory_order_release);
                return;
            }
        }
    }
};
