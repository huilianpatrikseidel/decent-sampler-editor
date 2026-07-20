set(PLUGIN_NAME WaveFolder)

set(SOURCE_FILES
    WaveFolderProcessor.cpp
    WaveFolderProcessor.h
    WaveFolderController.cpp
    WaveFolderController.h
    
    factory.cpp
)

add_sampler_plugin(${PLUGIN_NAME} "${SOURCE_FILES}")
