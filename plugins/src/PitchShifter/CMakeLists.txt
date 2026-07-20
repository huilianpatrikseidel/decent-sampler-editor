set(PLUGIN_NAME PitchShifter)

set(SOURCE_FILES
    PitchShifterProcessor.cpp
    PitchShifterProcessor.h
    PitchShifterController.cpp
    PitchShifterController.h
    
    factory.cpp
)

add_sampler_plugin(${PLUGIN_NAME} "${SOURCE_FILES}")
