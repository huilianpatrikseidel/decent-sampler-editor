set(PLUGIN_NAME StereoSimulator)

set(SOURCE_FILES
    StereoSimulatorProcessor.cpp
    StereoSimulatorProcessor.h
    StereoSimulatorController.cpp
    StereoSimulatorController.h
    
    factory.cpp
)

add_sampler_plugin(${PLUGIN_NAME} "${SOURCE_FILES}")
