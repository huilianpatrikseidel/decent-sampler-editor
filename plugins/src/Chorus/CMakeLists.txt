set(PLUGIN_NAME Chorus)

set(SOURCE_FILES
    ChorusProcessor.cpp
    ChorusProcessor.h
    ChorusController.cpp
    ChorusController.h
    ChorusUIDs.h
    factory.cpp
)

add_sampler_plugin(${PLUGIN_NAME} "${SOURCE_FILES}")
