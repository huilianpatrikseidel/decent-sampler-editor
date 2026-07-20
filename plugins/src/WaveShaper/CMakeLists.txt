set(PLUGIN_NAME WaveShaper)

set(SOURCE_FILES
    WaveShaperProcessor.cpp
    WaveShaperProcessor.h
    WaveShaperController.cpp
    WaveShaperController.h
    
    factory.cpp
)

add_sampler_plugin(${PLUGIN_NAME} "${SOURCE_FILES}")
