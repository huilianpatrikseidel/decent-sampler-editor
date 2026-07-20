set(PLUGIN_NAME Equalizer)

set(SOURCE_FILES
    EqualizerProcessor.cpp
    EqualizerProcessor.h
    EqualizerController.cpp
    EqualizerController.h
    SpectrumCurveView.cpp
    SpectrumCurveView.h
    
    factory.cpp
)

add_sampler_plugin(${PLUGIN_NAME} "${SOURCE_FILES}")
set_target_properties(${PLUGIN_NAME} PROPERTIES AUTOMOC OFF)
