set(PLUGIN_NAME Filter)

set(SOURCE_FILES
    FilterProcessor.cpp
    FilterProcessor.h
    FilterController.cpp
    FilterController.h
    
    factory.cpp
)

add_sampler_plugin(${PLUGIN_NAME} "${SOURCE_FILES}")
