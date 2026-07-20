set(PLUGIN_NAME Delay)

set(SOURCE_FILES
    DelayProcessor.cpp
    DelayProcessor.h
    DelayController.cpp
    DelayController.h
    
    factory.cpp
)

add_sampler_plugin(${PLUGIN_NAME} "${SOURCE_FILES}")
