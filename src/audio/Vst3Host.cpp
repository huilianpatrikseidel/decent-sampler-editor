#include "Vst3Host.h"
#include <QDebug>

#include "pluginterfaces/base/smartpointer.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivstmessage.h"
#include "pluginterfaces/gui/iplugview.h"
#include "public.sdk/source/vst/hosting/module.h"
#include "public.sdk/source/vst/utility/stringconvert.h"

using namespace Steinberg;

Vst3Host::Vst3Host() {}

Vst3Host::~Vst3Host() {
    unloadPlugin();
}

bool Vst3Host::loadPlugin(const QString& dllPath) {
    m_dllPath = dllPath;
    
    std::string errorDesc;
    m_module = VST3::Hosting::Module::create(dllPath.toStdString(), errorDesc);
    if (!m_module) {
        qWarning() << "Module::create failed for" << dllPath << ":" << QString::fromStdString(errorDesc);
        return false;
    }

    return initPlugin();
}

bool Vst3Host::initPlugin() {
    if (!m_module) {
        qWarning() << "initPlugin: m_module is null";
        return false;
    }

    auto factory = m_module->getFactory();
    auto classes = factory.classInfos();
    if (classes.empty()) {
        qWarning() << "initPlugin: classes is empty";
        return false;
    }

    // Find the first AudioEffect class
    VST3::UID effectClassID;
    bool foundEffect = false;
    for (const auto& cls : classes) {
        if (cls.category() == kVstAudioEffectClass) {
            effectClassID = cls.ID();
            foundEffect = true;
            break;
        }
    }

    if (!foundEffect) {
        qWarning() << "initPlugin: no AudioEffect class found";
        return false;
    }

    // Create the component
    IPtr<Vst::IComponent> component = factory.createInstance<Vst::IComponent>(effectClassID);
    if (!component) {
        qWarning() << "initPlugin: factory.createInstance failed";
        return false;
    }
    
    m_component = component;
    if (m_component) m_component->addRef();
    
    // Try to get audio processor
    m_component->queryInterface(Vst::IAudioProcessor::iid, (void**)&m_processor);

    // Try to get edit controller
    Vst::IEditController* controller = nullptr;
    if (m_component->queryInterface(Vst::IEditController::iid, (void**)&controller) == kResultTrue) {
        m_controller = controller;
        m_controller->setComponentHandler(this);
    } else {
        // If the component doesn't implement it, we might need to create it using the factory
        TUID controllerUID;
        if (m_component->getControllerClassId(controllerUID) == kResultTrue) {
            IPtr<Vst::IEditController> extController = factory.createInstance<Vst::IEditController>(VST3::UID(controllerUID));
            if (extController) {
                m_controller = extController;
                m_controller->setComponentHandler(this);
                m_controller->addRef();
            }
        }
    }

    if (m_component) {
        m_component->initialize(nullptr);
    }
    
    if (m_controller) {
        m_controller->initialize(nullptr);
        // Connect processor and controller if needed
        IPtr<Vst::IConnectionPoint> componentCP;
        IPtr<Vst::IConnectionPoint> controllerCP;
        
        if (m_component->queryInterface(Vst::IConnectionPoint::iid, (void**)&componentCP) == kResultTrue) {
            if (m_controller->queryInterface(Vst::IConnectionPoint::iid, (void**)&controllerCP) == kResultTrue) {
                componentCP->connect(controllerCP);
                controllerCP->connect(componentCP);
            }
        }
    }
    
    if (m_processor) {
        Vst::ProcessSetup setup;
        setup.processMode = Vst::kRealtime;
        setup.symbolicSampleSize = Vst::kSample32;
        setup.maxSamplesPerBlock = 1024;
        setup.sampleRate = 44100.0;
        
        m_processor->setupProcessing(setup);
    }
    
    if (m_component) m_component->setActive(true);
    if (m_processor) m_processor->setProcessing(true);

    if (m_processor == nullptr) {
        qWarning() << "initPlugin: m_processor is null";
    }

    return (m_processor != nullptr);
}

void Vst3Host::unloadPlugin() {
    closeUI();
    if (m_processor) m_processor->setProcessing(false);
    if (m_component) m_component->setActive(false);
    
    if (m_controller) {
        m_controller->release();
        m_controller = nullptr;
    }
    if (m_processor) {
        m_processor->release();
        m_processor = nullptr;
    }
    if (m_component) {
        m_component->release();
        m_component = nullptr;
    }
    m_module = nullptr;
}

void Vst3Host::processAudio(float** inputs, int numInputs, float** outputs, int numOutputs, int numSamples) {
    if (m_processor) {
        Vst::ProcessData data = {};
        data.numSamples = numSamples;
        data.symbolicSampleSize = Vst::kSample32;
        data.processMode = Vst::kRealtime;
        
        Vst::AudioBusBuffers inputBus = {};
        inputBus.numChannels = numInputs;
        inputBus.channelBuffers32 = inputs;
        inputBus.silenceFlags = 0;
        
        Vst::AudioBusBuffers outputBus = {};
        outputBus.numChannels = numOutputs;
        outputBus.channelBuffers32 = outputs;
        outputBus.silenceFlags = 0;
        
        data.numInputs = 1;
        data.inputs = &inputBus;
        
        data.numOutputs = 1;
        data.outputs = &outputBus;
        
        m_processor->process(data);
    }
}

void Vst3Host::setParameter(int paramId, float normalizedValue) {
    if (m_controller) {
        m_controller->setParamNormalized(paramId, normalizedValue);
    }
}

float Vst3Host::getParameter(int paramId) const {
    if (m_controller) {
        return m_controller->getParamNormalized(paramId);
    }
    return 0.0f;
}

void Vst3Host::openUI(void* parentWindowHandle) {
    if (!m_controller) return;

    if (!m_view) {
        IPlugView* view = m_controller->createView(Vst::ViewType::kEditor);
        if (view) {
            m_view = view;
            m_view->setFrame(this);
        }
    }

    if (m_view && parentWindowHandle) {
        m_view->attached(parentWindowHandle, kPlatformTypeHWND);
        
        ViewRect rect;
        if (m_view->getSize(&rect) == kResultTrue) {
            m_view->onSize(&rect);
        }
    }
}

void Vst3Host::resizeUI(int width, int height) {
    if (m_view) {
        ViewRect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = width;
        rect.bottom = height;
        m_view->onSize(&rect);
    }
}

void Vst3Host::closeUI() {
    if (m_view) {
        m_view->removed();
        m_view->release();
        m_view = nullptr;
    }
}

Steinberg::tresult PLUGIN_API Vst3Host::queryInterface(const Steinberg::TUID _iid, void** obj) {
    if (memcmp(_iid, Steinberg::IPlugFrame::iid, sizeof(Steinberg::TUID)) == 0) {
        *obj = static_cast<Steinberg::IPlugFrame*>(this);
        return Steinberg::kResultTrue;
    }
    if (memcmp(_iid, Steinberg::Vst::IComponentHandler::iid, sizeof(Steinberg::TUID)) == 0) {
        *obj = static_cast<Steinberg::Vst::IComponentHandler*>(this);
        return Steinberg::kResultTrue;
    }
    if (memcmp(_iid, Steinberg::FUnknown::iid, sizeof(Steinberg::TUID)) == 0) {
        *obj = static_cast<Steinberg::FUnknown*>(static_cast<Steinberg::IPlugFrame*>(this));
        return Steinberg::kResultTrue;
    }
    *obj = nullptr;
    return Steinberg::kNoInterface;
}

Steinberg::tresult PLUGIN_API Vst3Host::resizeView(Steinberg::IPlugView* view, Steinberg::ViewRect* newSize) {
    if (view && newSize) {
        return view->onSize(newSize);
    }
    return Steinberg::kResultFalse;
}

