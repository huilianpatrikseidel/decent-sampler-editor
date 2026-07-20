set(PLUGIN_NAME Gain)

set(SOURCE_FILES
    GainProcessor.cpp
    GainProcessor.h
    GainController.cpp
    GainController.h
    
    factory.cpp
)

add_sampler_plugin(${PLUGIN_NAME} "${SOURCE_FILES}")
