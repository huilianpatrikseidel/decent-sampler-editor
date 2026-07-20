set(PLUGIN_NAME BitCrusher)

set(SOURCE_FILES
    BitCrusherProcessor.cpp
    BitCrusherProcessor.h
    BitCrusherController.cpp
    BitCrusherController.h
    
    factory.cpp
)

add_sampler_plugin(${PLUGIN_NAME} "${SOURCE_FILES}")
