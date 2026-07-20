#pragma once

#include <QString>
#include <QUuid>
#include <memory>
#include <string>

// Forward declarations for Steinberg VST3 interfaces
namespace VST3 {
    namespace Hosting {
        class Module;
    }
}

namespace Steinberg {
    class IPlugView;
    namespace Vst {
        class IComponent;
        class IEditController;
        class IAudioProcessor;
    }
}

#include "pluginterfaces/gui/iplugview.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"

#include "Vst3HostExport.h"

class VST3HOST_EXPORT Vst3Host : public Steinberg::IPlugFrame, public Steinberg::Vst::IComponentHandler {
public:
    Vst3Host();
    virtual ~Vst3Host();

    // IPlugFrame implementation
    Steinberg::tresult PLUGIN_API resizeView(Steinberg::IPlugView* view, Steinberg::ViewRect* newSize) override;
    
    // IComponentHandler implementation
    Steinberg::tresult PLUGIN_API beginEdit(Steinberg::Vst::ParamID id) override { return Steinberg::kResultTrue; }
    Steinberg::tresult PLUGIN_API performEdit(Steinberg::Vst::ParamID id, Steinberg::Vst::ParamValue valueNormalized) override { return Steinberg::kResultTrue; }
    Steinberg::tresult PLUGIN_API endEdit(Steinberg::Vst::ParamID id) override { return Steinberg::kResultTrue; }
    Steinberg::tresult PLUGIN_API restartComponent(Steinberg::int32 flags) override { return Steinberg::kResultTrue; }

    // FUnknown implementation
    Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID _iid, void** obj) override;
    Steinberg::uint32 PLUGIN_API addRef() override { return 1; }
    Steinberg::uint32 PLUGIN_API release() override { return 1; }

    bool loadPlugin(const QString& dllPath);
    void unloadPlugin();

    void processAudio(float** inputs, int numInputs, float** outputs, int numOutputs, int numSamples);

    void setParameter(int paramId, float normalizedValue);
    float getParameter(int paramId) const;

    void openUI(void* parentWindowHandle);
    void closeUI();
    void resizeUI(int width, int height);

private:
    QString m_dllPath;

    std::shared_ptr<VST3::Hosting::Module> m_module;
    Steinberg::Vst::IComponent* m_component = nullptr;
    Steinberg::Vst::IEditController* m_controller = nullptr;
    Steinberg::Vst::IAudioProcessor* m_processor = nullptr;
    Steinberg::IPlugView* m_view = nullptr;

    bool initPlugin();
};
