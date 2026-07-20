set(PLUGIN_NAME Compressor)

set(SOURCE_FILES
    CompressorProcessor.cpp
    CompressorProcessor.h
    CompressorController.cpp
    CompressorController.h
    
    factory.cpp
)

add_sampler_plugin(${PLUGIN_NAME} "${SOURCE_FILES}")
