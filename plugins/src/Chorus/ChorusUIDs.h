#pragma once
#include "pluginterfaces/base/funknown.h"

namespace SamplerEditor {
namespace Chorus {

// Randomly generated valid FUIDs
static const Steinberg::FUID ChorusProcessorUID (0x8F5F2B1E, 0x9B1C4B59, 0xA3D6F7E8, 0x1C2D3E4F);
static const Steinberg::FUID ChorusControllerUID (0x7E4D3C2B, 0x1A0F9E8D, 0x7C6B5A49, 0x3D2E1F0A);

enum {
    kRateId = 100,
    kDepthId = 101,
    kMixId = 102
};

}
}
