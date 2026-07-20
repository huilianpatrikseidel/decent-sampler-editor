cmake_minimum_required(VERSION 3.15)

project(SamplerEditorPlugins VERSION 1.0.0)

# Disable the automatic symlink creation which requires Admin rights on Windows
set(SMTG_CREATE_PLUGIN_LINK OFF CACHE BOOL "Disable plugin symlink" FORCE)

# Define a function to easily add our VST3 plugins
function(add_sampler_plugin PLUGIN_NAME SOURCE_FILES)
    # Disable the automatic symlink creation which requires Admin rights on Windows
    set(SMTG_CREATE_PLUGIN_LINK OFF)
    # Automatically generate an RC file to embed the uidesc on Windows (since we don't build as a .vst3 bundle folder)
    if(WIN32)
        set(RC_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_NAME}.rc")
        file(WRITE "${RC_FILE}" "\"${PLUGIN_NAME}.uidesc\" DATA \"${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN_NAME}.uidesc\"\n")
        set_property(SOURCE "${RC_FILE}" PROPERTY OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN_NAME}.uidesc")
        list(APPEND SOURCE_FILES "${RC_FILE}")
    endif()

    smtg_add_vst3plugin(${PLUGIN_NAME}
        ${SOURCE_FILES}
    )
    
    install(TARGETS ${PLUGIN_NAME} 
        RUNTIME DESTINATION "VST3" 
        LIBRARY DESTINATION "VST3"
    )
    
    smtg_target_add_plugin_resources(${PLUGIN_NAME} RESOURCES "${PLUGIN_NAME}.uidesc")

    if (WIN32)
        set_target_properties(${PLUGIN_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
        # On MinGW, VSTGUI needs explicit Windows libs
        target_link_libraries(${PLUGIN_NAME} PRIVATE
            sdk vstgui_support vstgui_uidescription vstgui sdk sdk_common base
            shlwapi opengl32 d3d11 dwmapi d2d1 dwrite windowscodecs
        )
        # MinGW linker drops dllmain.cpp from libsdk.a because it doesn't consider DllMain a strong reference.
        # This causes 'undefined reference to moduleHandle' in moduleinit.cpp.
        target_sources(${PLUGIN_NAME} PRIVATE 
            ${CMAKE_SOURCE_DIR}/build/_deps/vst3sdk-src/public.sdk/source/main/dllmain.cpp
        )
    else()
        target_link_libraries(${PLUGIN_NAME} PRIVATE
            sdk vstgui_support vstgui_uidescription vstgui
        )
    endif()
endfunction()

# We will add each plugin here once we create their source files
add_subdirectory(src/Chorus)
add_subdirectory(src/Compressor)
add_subdirectory(src/Delay)
add_subdirectory(src/Equalizer)
add_subdirectory(src/Filter)
add_subdirectory(src/Reverb)
add_subdirectory(src/WaveShaper)
add_subdirectory(src/Gain)
add_subdirectory(src/Phaser)
add_subdirectory(src/PitchShifter)
add_subdirectory(src/WaveFolder)
add_subdirectory(src/StereoSimulator)
add_subdirectory(src/BitCrusher)