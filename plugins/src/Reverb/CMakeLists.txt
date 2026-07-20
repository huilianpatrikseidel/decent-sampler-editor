set(PLUGIN_NAME Reverb)

set(SOURCE_FILES
    ReverbProcessor.cpp
    ReverbProcessor.h
    ReverbController.cpp
    ReverbController.h
    
    factory.cpp
)

add_sampler_plugin(${PLUGIN_NAME} "${SOURCE_FILES}")
