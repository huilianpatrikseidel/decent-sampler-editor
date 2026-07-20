set(PLUGIN_NAME Phaser)

set(SOURCE_FILES
    PhaserProcessor.cpp
    PhaserProcessor.h
    PhaserController.cpp
    PhaserController.h
    
    factory.cpp
)

add_sampler_plugin(${PLUGIN_NAME} "${SOURCE_FILES}")
