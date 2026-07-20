cmake_minimum_required(VERSION 3.20)
project(SamplerEditor VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC_COMPILER_PREDEFINES OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg SvgWidgets Sql Test)

include(FetchContent)
FetchContent_Declare(
    RtMidi
    GIT_REPOSITORY https://github.com/thestk/rtmidi.git
    GIT_TAG master
)
set(RTMIDI_API_WINMM ON CACHE BOOL "Enable WINMM API for RtMidi" FORCE)
set(RTMIDI_BUILD_SHARED_LIBS ON CACHE BOOL "Build rtmidi as shared library" FORCE)
set(RTMIDI_BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(RtMidi)

FetchContent_Declare(
    concurrentqueue
    GIT_REPOSITORY https://github.com/cameron314/concurrentqueue.git
    GIT_TAG master
)
FetchContent_MakeAvailable(concurrentqueue)

FetchContent_Declare(
    miniz
    GIT_REPOSITORY https://github.com/richgel999/miniz.git
    GIT_TAG master
)
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(miniz)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

enable_testing()

# VST3 SDK
FetchContent_Declare(
    vst3sdk
    GIT_REPOSITORY https://github.com/steinbergmedia/vst3sdk.git
    GIT_TAG        v3.7.12_build_20
    GIT_SHALLOW    ON
    GIT_PROGRESS   ON
    GIT_SUBMODULES_RECURSE ON
)
FetchContent_GetProperties(vst3sdk)
if(NOT vst3sdk_POPULATED)
    FetchContent_Populate(vst3sdk)
    set(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES OFF CACHE BOOL "Build Examples" FORCE)
    set(SMTG_ENABLE_VST3_HOSTING_EXAMPLES OFF CACHE BOOL "Build VST3 Examples" FORCE)
    set(SMTG_ENABLE_VSTGUI_SUPPORT ON CACHE BOOL "Add VSTGUI" FORCE)
    set(SMTG_BUILD_TESTS OFF CACHE BOOL "Build Tests" FORCE)
    set(SMTG_BUILD_VST3_TESTS OFF CACHE BOOL "Build VST3 Tests" FORCE)

    add_subdirectory(${vst3sdk_SOURCE_DIR} ${vst3sdk_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# Set the SDK root manually
set(VST3_SDK_ROOT ${vst3sdk_SOURCE_DIR})

set(SMTG_ADD_VST3_HOSTING_SAMPLES OFF CACHE BOOL "" FORCE)
set(SMTG_ADD_VST3_PLUGIN_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SMTG_RUN_VST_VALIDATOR OFF CACHE BOOL "" FORCE)
set(SMTG_CREATE_BUNDLE_FOR_WINDOWS OFF CACHE BOOL "" FORCE)
set(SMTG_ADD_VSTGUI OFF CACHE BOOL "" FORCE)
set(SMTG_MYPLUGINS_SRC_PATH "" CACHE STRING "" FORCE)
set(SMTG_ENABLE_VST3_HOSTING_ARCHITECTURE_ONLY OFF CACHE BOOL "" FORCE)
set(SMTG_COREAUDIO_SDK_PATH "" CACHE PATH "" FORCE)
set(SMTG_ENABLE_TARGET_VARS OFF CACHE BOOL "" FORCE)
set(SMTG_CREATE_PLUGIN_MAC_BUNDLE OFF CACHE BOOL "" FORCE)
set(SMTG_MAC_CODE_SIGN_IDENTITY "" CACHE STRING "" FORCE)

find_path(CONCURRENTQUEUE_INCLUDE_DIRS "concurrentqueue.h")

set(LIB_SOURCES
    src/ui/waveform/WaveformCache.cpp
)

add_library(SamplerEditorLib STATIC ${LIB_SOURCES})
target_include_directories(SamplerEditorLib PUBLIC src)

add_library(MiniaudioLib SHARED src/thirdparty/miniaudio/miniaudio.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/MiniaudioLib.rc
)
add_dependencies(MiniaudioLib UpdateBuildNumber)
set_target_properties(MiniaudioLib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON AUTOMOC OFF AUTOUIC OFF)
target_include_directories(MiniaudioLib PUBLIC src/thirdparty/miniaudio)

add_library(FFTConvolverLib SHARED 
    src/thirdparty/FFTConvolver/FFTConvolver.cpp
    src/thirdparty/FFTConvolver/TwoStageFFTConvolver.cpp
    src/thirdparty/FFTConvolver/Utilities.cpp
    src/thirdparty/FFTConvolver/AudioFFT.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/FFTConvolverLib.rc
)
add_dependencies(FFTConvolverLib UpdateBuildNumber)
set_target_properties(FFTConvolverLib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON AUTOMOC OFF AUTOUIC OFF)
target_include_directories(FFTConvolverLib PUBLIC src/thirdparty/FFTConvolver)
add_library(Vst3HostLib SHARED
    src/audio/Vst3Host.cpp
    ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/module.cpp
    ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/module_win32.cpp
    ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/hostclasses.cpp
    ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/plugprovider.cpp
    ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/pluginterfacesupport.cpp
    ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/connectionproxy.cpp
    ${VST3_SDK_ROOT}/public.sdk/source/common/threadchecker_win32.cpp
    ${VST3_SDK_ROOT}/pluginterfaces/base/funknown.cpp
    ${VST3_SDK_ROOT}/pluginterfaces/base/ustring.cpp
    ${VST3_SDK_ROOT}/pluginterfaces/base/coreiids.cpp
    ${VST3_SDK_ROOT}/base/source/baseiids.cpp
    ${VST3_SDK_ROOT}/base/source/fstring.cpp
    ${VST3_SDK_ROOT}/base/source/fobject.cpp
    ${VST3_SDK_ROOT}/base/thread/source/flock.cpp
    ${VST3_SDK_ROOT}/base/source/updatehandler.cpp
    ${VST3_SDK_ROOT}/public.sdk/source/vst/utility/stringconvert.cpp
    ${VST3_SDK_ROOT}/public.sdk/source/common/commonstringconvert.cpp
    ${VST3_SDK_ROOT}/public.sdk/source/vst/vstinitiids.cpp
    ${VST3_SDK_ROOT}/public.sdk/source/common/commoniids.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/Vst3HostLib.rc
)
add_dependencies(Vst3HostLib UpdateBuildNumber)
target_include_directories(Vst3HostLib PUBLIC ${VST3_SDK_ROOT})
set_target_properties(Vst3HostLib PROPERTIES AUTOMOC OFF AUTOUIC OFF WINDOWS_EXPORT_ALL_SYMBOLS ON)
target_compile_definitions(Vst3HostLib PRIVATE RELEASE NDEBUG VST3HOST_BUILD)
target_link_libraries(Vst3HostLib PUBLIC Qt6::Core)
target_link_libraries(Vst3HostLib PRIVATE pluginterfaces base)

add_library(AudioEngineLib SHARED
    src/audio/AudioEngine.cpp
    src/audio/PlaybackSequencer.cpp
    src/audio/dsp/AdsrEnvelope.cpp
    src/audio/dsp/LfoOscillator.cpp
    src/audio/dsp/Oscillator.cpp
    src/audio/dsp/StateVariableFilter.cpp
    src/audio/dsp/VoiceProcessor.cpp
    src/audio/AudioAnalyzer.cpp
    src/audio/MidiEngine.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/AudioEngineLib.rc
)
add_dependencies(AudioEngineLib UpdateBuildNumber)
set_target_properties(AudioEngineLib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON AUTOMOC ON AUTOUIC OFF AUTOMOC_COMPILER_PREDEFINES OFF)
target_compile_definitions(AudioEngineLib PRIVATE AUDIOENGINE_BUILD)
target_include_directories(AudioEngineLib PUBLIC src)
target_link_libraries(AudioEngineLib PUBLIC
    Qt6::Core
    MiniaudioLib
    FFTConvolverLib
    rtmidi
    concurrentqueue
    Vst3HostLib
)

add_library(CoreLib SHARED
    src/core/models/ProjectStates.cpp
    src/core/ProjectManager.cpp
    src/core/ProjectSerializer.cpp
    src/core/serializers/AudioStateSerializer.cpp
    src/core/serializers/UiStateSerializer.cpp
    src/core/serializers/GraphSerializer.cpp
    src/core/AppSettings.cpp
    src/core/PresetManager.cpp
    src/core/factories/NodeFactory.cpp
    src/core/factories/UiComponentFactory.cpp
    src/core/models/AudioNodes.h
    src/core/models/GraphStructures.h
    src/core/models/UiComponents.h
    src/core/AppSettings.cpp
    src/core/AutoMapper.cpp
    src/core/DsDom.cpp
    src/core/ApplicationController.cpp
    src/core/AudioGraphBuilder.cpp
    src/core/VstPluginManager.cpp
    src/core/DatabaseManager.cpp
    src/core/ThemePalette.cpp
    src/commands/AddNodeCommand.cpp
    src/commands/MoveNodeCommand.cpp
    src/commands/ModifyPropertyCommand.cpp
    src/commands/RemoveNodeCommand.cpp
    src/commands/ConnectionCommands.cpp
    src/commands/UiCommands.cpp
    src/commands/ZoneCommands.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/CoreLib.rc
)
add_dependencies(CoreLib UpdateBuildNumber)
set_target_properties(CoreLib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
target_compile_definitions(CoreLib PRIVATE CORELIB_BUILD)
target_include_directories(CoreLib PUBLIC src)
target_link_libraries(CoreLib PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    AudioEngineLib
)

add_library(TranspilersLib SHARED
    src/transpilers/DecentSamplerTranspiler.cpp
    src/transpilers/ds/DsGroupBuilder.cpp
    src/transpilers/ds/DsEffectBuilder.cpp
    src/transpilers/ds/DsUiBuilder.cpp
    src/transpilers/SfzTranspiler.cpp
    src/export/BundleExporter.cpp
    src/export/ExportWorker.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/TranspilersLib.rc
)
add_dependencies(TranspilersLib UpdateBuildNumber)
set_target_properties(TranspilersLib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON AUTOMOC ON AUTOUIC OFF AUTOMOC_COMPILER_PREDEFINES OFF)
target_include_directories(TranspilersLib PUBLIC src)
target_link_libraries(TranspilersLib PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    miniz
    CoreLib
)

add_library(NodeMapLib SHARED
    src/ui/nodemap/NodeMapView.cpp
    src/ui/canvas/NodeGraphView.cpp
    src/ui/canvas/GraphNodeItem.cpp
    src/ui/canvas/GraphConnectionItem.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/NodeMapLib.rc
)
add_dependencies(NodeMapLib UpdateBuildNumber)
set_target_properties(NodeMapLib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON AUTOMOC ON)
target_compile_definitions(NodeMapLib PRIVATE NODEMAP_BUILD)
target_include_directories(NodeMapLib PUBLIC src)
target_link_libraries(NodeMapLib PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets CoreLib)

add_library(UiDesignerLib SHARED
    src/ui/designer/UIDesignerView.cpp
    src/ui/canvas/InstrumentCanvasView.cpp
    src/ui/canvas/CanvasHeaderItem.cpp
    src/ui/canvas/CanvasKeyboardItem.cpp
    src/ui/canvas/CanvasKeyboardPainter.cpp
    src/ui/canvas/CanvasContextMenu.cpp
    src/ui/canvas/CanvasDragDropHandler.cpp
    src/ui/canvas/FilmstripRenderer.cpp
    src/ui/canvas/UiComponentItem.cpp
    src/ui/canvas/UiComponentItemFactory.cpp
    src/ui/canvas/UiComponentItems.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/UiDesignerLib.rc
)
add_dependencies(UiDesignerLib UpdateBuildNumber)
set_target_properties(UiDesignerLib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON AUTOMOC ON)
target_compile_definitions(UiDesignerLib PRIVATE UIDESIGNER_BUILD)
target_include_directories(UiDesignerLib PUBLIC src)
target_link_libraries(UiDesignerLib PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::SvgWidgets CoreLib)

add_library(UiComponentsLib SHARED
    src/ui/components/CustomTitleBar.cpp
    src/ui/components/CurveGraphWidget.cpp
    src/ui/components/KnobWidget.cpp
    src/ui/components/SynthKnobWidget.cpp
    src/ui/components/FaderWidget.cpp
    src/ui/components/SvgToggleWidget.cpp
    src/ui/components/ModSourceWidget.cpp
    src/ui/components/ModSourceHelper.cpp
    src/ui/components/ModRoutingWidget.cpp
    src/ui/components/RangeSliderWidget.cpp
    src/ui/components/RoutingPopupWidget.cpp
    src/ui/components/EnvelopeGraphWidget.cpp
    src/ui/components/LFOGraphWidget.cpp
    src/ui/components/WavetableViewerWidget.cpp
    src/ui/components/WaveformSelectorWidget.cpp
    src/ui/components/FilterSectionWidget.cpp
    src/ui/components/NeumorphicMessageBox.cpp
    src/ui/components/NeumorphicDialog.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/UiComponentsLib.rc
)
add_dependencies(UiComponentsLib UpdateBuildNumber)
set_target_properties(UiComponentsLib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON AUTOMOC ON)
target_compile_definitions(UiComponentsLib PRIVATE UICOMPONENTSLIB_BUILD)
target_include_directories(UiComponentsLib PUBLIC src)
target_link_libraries(UiComponentsLib PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::SvgWidgets CoreLib)

add_library(MapperLib SHARED
    src/ui/mapper/MappingEditorContainer.cpp
    src/ui/mapper/MappingEditorView.cpp
    src/ui/mapper/MappingGridPainter.cpp
    src/ui/mapper/ZoneItem.cpp
    src/ui/mapper/WaveformView.cpp
    src/ui/mapper/WaveformAudioLoader.cpp
    src/ui/mapper/ZoneImportHandler.cpp
    src/ui/mapper/ZoneImportOverlay.cpp
    src/ui/mapper/GroupDialogs.cpp
    src/ui/mapper/cards/SynthCardBase.cpp
    src/ui/mapper/cards/OscillatorCardWidget.cpp
    src/ui/mapper/cards/FilterCardWidget.cpp
    src/ui/mapper/cards/EnvelopeCardWidget.cpp
    src/ui/mapper/cards/GlobalLfoCardWidget.cpp
    src/ui/mapper/cards/LegatoCardWidget.cpp
    src/ui/mapper/SynthEditorView.cpp
    src/ui/mapper/MacroPanelWidget.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/MapperLib.rc
)
add_dependencies(MapperLib UpdateBuildNumber)
set_target_properties(MapperLib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON AUTOMOC ON)
target_compile_definitions(MapperLib PRIVATE MAPPERLIB_BUILD)
target_include_directories(MapperLib PUBLIC src)
target_link_libraries(MapperLib PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::SvgWidgets CoreLib UiComponentsLib UiDesignerLib)

add_library(MixerLib SHARED
    src/ui/mixer/MixerView.cpp
    src/ui/mixer/MixerChannelStrip.cpp
    src/ui/mixer/FxRackWidget.cpp
    src/ui/mixer/EffectEditorDialog.cpp
    src/ui/mixer/FxSlotWidget.cpp
    src/ui/mixer/Vst3EditorWidget.cpp
    src/ui/mixer/fx_ui/FxKnobWidget.cpp
    src/ui/mixer/fx_ui/FxSwitchWidget.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/MixerLib.rc
)
add_dependencies(MixerLib UpdateBuildNumber)
set_target_properties(MixerLib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON AUTOMOC ON)
target_compile_definitions(MixerLib PRIVATE MIXERLIB_BUILD)
target_include_directories(MixerLib PUBLIC src)
target_link_libraries(MixerLib PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::SvgWidgets CoreLib UiComponentsLib)


target_link_libraries(SamplerEditorLib PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    miniz
    concurrentqueue
    rtmidi
    winmm
    Vst3HostLib
    MiniaudioLib
    FFTConvolverLib
    AudioEngineLib
    CoreLib
    TranspilersLib
    NodeMapLib
    UiDesignerLib
    UiComponentsLib
    MapperLib
    MixerLib
)

target_include_directories(SamplerEditorLib PUBLIC 
    src
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/_deps/vst3sdk-src
)

if (CONCURRENTQUEUE_INCLUDE_DIRS)
    target_include_directories(SamplerEditorLib PUBLIC ${CONCURRENTQUEUE_INCLUDE_DIRS})
endif()


# Executável Principal
set(APP_SOURCES 
    src/main.cpp 
    src/ui/MainWindow.cpp
    src/ui/AboutDialog.cpp
    src/ui/SplashScreen.cpp
    src/ui/library/LibraryView.cpp
    src/ui/code/CodeEditorView.cpp
    src/ui/core/ThemeManager.cpp
    src/ui/code/SyntaxHighlighter.cpp
    src/ui/core/MenuManager.cpp
    src/ui/core/DockManager.cpp
    src/ui/core/FocusModeOverlay.cpp
    src/ui/editor/SampleEditorContainer.cpp
    src/ui/editor/GroupEditorView.cpp
    src/ui/editor/ModulatorsView.cpp
    src/ui/waveform/WaveformEditorView.cpp
    src/ui/inspector/PropertiesInspector.cpp
    src/ui/inspector/strategies/NodeInspectorStrategy.cpp
    src/ui/inspector/strategies/UiComponentInspectorStrategy.cpp
    src/ui/inspector/strategies/ZoneInspectorStrategy.cpp
    src/ui/inspector/strategies/CanvasInspectorStrategy.cpp
    src/ui/inspector/UiPropertyForms.cpp
    src/ui/inspector/NodePropertyForms.cpp
    src/ui/inspector/AdsrEditorView.cpp
    src/ui/core/PresetListWidget.cpp
    src/ui/core/ProjectTreeWidget.cpp
    src/ui/assets/AssetManagerView.cpp
    src/ui/sequencer/NoteSequenceEditorView.cpp
    src/ui/sequencer/SequenceGraphWidget.cpp
    src/ui/SettingsDialog.cpp
    src/core/InitWorker.cpp
    src/resources.qrc
    ${CMAKE_CURRENT_BINARY_DIR}/app-icon.rc
)

add_custom_target(UpdateBuildNumber
    COMMAND ${CMAKE_COMMAND} 
    -DSRC_DIR=${CMAKE_CURRENT_SOURCE_DIR}
    -DBIN_DIR=${CMAKE_CURRENT_BINARY_DIR}
    -DPROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    -DPROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    -DPROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/increment_build.cmake
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/build_version.cpp ${CMAKE_CURRENT_BINARY_DIR}/app-icon.rc 
    ${CMAKE_CURRENT_BINARY_DIR}/AudioEngineLib.rc ${CMAKE_CURRENT_BINARY_DIR}/CoreLib.rc ${CMAKE_CURRENT_BINARY_DIR}/TranspilersLib.rc
    ${CMAKE_CURRENT_BINARY_DIR}/NodeMapLib.rc ${CMAKE_CURRENT_BINARY_DIR}/UiDesignerLib.rc ${CMAKE_CURRENT_BINARY_DIR}/UiComponentsLib.rc
    ${CMAKE_CURRENT_BINARY_DIR}/MapperLib.rc ${CMAKE_CURRENT_BINARY_DIR}/MixerLib.rc
    ${CMAKE_CURRENT_BINARY_DIR}/MiniaudioLib.rc ${CMAKE_CURRENT_BINARY_DIR}/FFTConvolverLib.rc ${CMAKE_CURRENT_BINARY_DIR}/Vst3HostLib.rc
    ${CMAKE_CURRENT_BINARY_DIR}/rtmidi.rc ${CMAKE_CURRENT_BINARY_DIR}/miniz.rc
)

# Important: Tell CMake these files are generated so it doesn't complain they are missing during config
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/build_version.cpp ${CMAKE_CURRENT_BINARY_DIR}/app-icon.rc 
    ${CMAKE_CURRENT_BINARY_DIR}/AudioEngineLib.rc ${CMAKE_CURRENT_BINARY_DIR}/CoreLib.rc ${CMAKE_CURRENT_BINARY_DIR}/TranspilersLib.rc
    ${CMAKE_CURRENT_BINARY_DIR}/NodeMapLib.rc ${CMAKE_CURRENT_BINARY_DIR}/UiDesignerLib.rc ${CMAKE_CURRENT_BINARY_DIR}/UiComponentsLib.rc
    ${CMAKE_CURRENT_BINARY_DIR}/MapperLib.rc ${CMAKE_CURRENT_BINARY_DIR}/MixerLib.rc
    ${CMAKE_CURRENT_BINARY_DIR}/MiniaudioLib.rc ${CMAKE_CURRENT_BINARY_DIR}/FFTConvolverLib.rc ${CMAKE_CURRENT_BINARY_DIR}/Vst3HostLib.rc
    ${CMAKE_CURRENT_BINARY_DIR}/rtmidi.rc ${CMAKE_CURRENT_BINARY_DIR}/miniz.rc
    PROPERTIES GENERATED TRUE)

file(TOUCH ${CMAKE_CURRENT_BINARY_DIR}/rtmidi.rc)
file(TOUCH ${CMAKE_CURRENT_BINARY_DIR}/miniz.rc)

target_sources(rtmidi PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/rtmidi.rc)
add_dependencies(rtmidi UpdateBuildNumber)

target_sources(miniz PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/miniz.rc)
add_dependencies(miniz UpdateBuildNumber)

add_executable(DecentSamplerEditor WIN32 ${APP_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/build_version.cpp)
add_dependencies(DecentSamplerEditor UpdateBuildNumber)
target_link_libraries(DecentSamplerEditor PRIVATE SamplerEditorLib Qt6::Svg Qt6::SvgWidgets)

# --- Copiar DLLs para a pasta do executável ---
# 1. Copiar as DLLs (RtMidi, miniz)
add_custom_command(TARGET DecentSamplerEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:rtmidi>"
    "$<TARGET_FILE_DIR:DecentSamplerEditor>"
    COMMENT "Copiando librtmidi.dll para a pasta de build..."
)

add_custom_command(TARGET DecentSamplerEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:miniz>"
    "$<TARGET_FILE_DIR:DecentSamplerEditor>"
    COMMENT "Copiando miniz.dll para a pasta de build..."
)

add_custom_command(TARGET DecentSamplerEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:MiniaudioLib>"
    "$<TARGET_FILE_DIR:DecentSamplerEditor>"
    COMMENT "Copiando MiniaudioLib.dll para a pasta de build..."
)

add_custom_command(TARGET DecentSamplerEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:FFTConvolverLib>"
    "$<TARGET_FILE_DIR:DecentSamplerEditor>"
    COMMENT "Copiando FFTConvolverLib.dll para a pasta de build..."
)

add_custom_command(TARGET DecentSamplerEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:Vst3HostLib>"
    "$<TARGET_FILE_DIR:DecentSamplerEditor>"
    COMMENT "Copiando Vst3HostLib.dll para a pasta de build..."
)

add_custom_command(TARGET DecentSamplerEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:AudioEngineLib>"
    "$<TARGET_FILE_DIR:DecentSamplerEditor>"
    COMMENT "Copiando AudioEngineLib.dll para a pasta de build..."
)

add_custom_command(TARGET DecentSamplerEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:CoreLib>"
    "$<TARGET_FILE_DIR:DecentSamplerEditor>"
    COMMENT "Copiando CoreLib.dll para a pasta de build..."
)

add_custom_command(TARGET DecentSamplerEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:TranspilersLib>"
    "$<TARGET_FILE_DIR:DecentSamplerEditor>"
    COMMENT "Copiando TranspilersLib.dll para a pasta de build..."
)

add_custom_command(TARGET DecentSamplerEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:NodeMapLib>"
    "$<TARGET_FILE_DIR:DecentSamplerEditor>"
    COMMENT "Copiando NodeMapLib.dll para a pasta de build..."
)

add_custom_command(TARGET DecentSamplerEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:UiDesignerLib>"
    "$<TARGET_FILE_DIR:DecentSamplerEditor>"
    COMMENT "Copiando UiDesignerLib.dll para a pasta de build..."
)

add_custom_command(TARGET DecentSamplerEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:UiComponentsLib>"
    "$<TARGET_FILE_DIR:DecentSamplerEditor>"
    COMMENT "Copiando UiComponentsLib.dll para a pasta de build..."
)

add_custom_command(TARGET DecentSamplerEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:MapperLib>"
    "$<TARGET_FILE_DIR:DecentSamplerEditor>"
    COMMENT "Copiando MapperLib.dll para a pasta de build..."
)

add_custom_command(TARGET DecentSamplerEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:MixerLib>"
    "$<TARGET_FILE_DIR:DecentSamplerEditor>"
    COMMENT "Copiando MixerLib.dll para a pasta de build..."
)

# 2. Utilizar windeployqt para copiar as dependências do Qt (e do compilador)
get_target_property(_qt_bin_dir Qt6::Core LOCATION)
get_filename_component(_qt_bin_dir "${_qt_bin_dir}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

if(WIN32 AND WINDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET DecentSamplerEditor POST_BUILD
        COMMAND ${WINDEPLOYQT_EXECUTABLE} --no-translations "$<TARGET_FILE:DecentSamplerEditor>"
        COMMENT "Rodando windeployqt para copiar as DLLs do Qt..."
    )
endif()

add_subdirectory(tests)

add_executable(svg2ico svg2ico.cpp)
target_link_libraries(svg2ico Qt6::Gui Qt6::Svg)

add_subdirectory(plugins)

# Set output names and prefixes for DLL targets
set(THIRD_PARTY_DLLS rtmidi miniz MiniaudioLib FFTConvolverLib Vst3HostLib)
set(INTERNAL_DLLS AudioEngineLib CoreLib TranspilersLib NodeMapLib UiDesignerLib UiComponentsLib MapperLib MixerLib)

foreach(TARGET ${THIRD_PARTY_DLLS})
    string(REGEX REPLACE "Lib$" "" BASENAME "${TARGET}")
    set_target_properties(${TARGET} PROPERTIES PREFIX "" OUTPUT_NAME "${BASENAME}")
endforeach()

foreach(TARGET ${INTERNAL_DLLS})
    string(REGEX REPLACE "Lib$" "" COMPONENT_NAME "${TARGET}")
    set_target_properties(${TARGET} PROPERTIES PREFIX "" OUTPUT_NAME "ds${COMPONENT_NAME}")
endforeach()

# --- Deploy (Install) ---
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/Deploy" CACHE PATH "Install prefix" FORCE)

install(TARGETS DecentSamplerEditor
    RUNTIME DESTINATION "."
)

# Install rtmidi DLL if built as shared
if(TARGET rtmidi)
    install(TARGETS rtmidi RUNTIME DESTINATION ".")
endif()

# Install miniz DLL if built as shared
if(TARGET miniz)
    install(TARGETS miniz RUNTIME DESTINATION ".")
endif()

if(TARGET MiniaudioLib)
    install(TARGETS MiniaudioLib RUNTIME DESTINATION ".")
endif()

if(TARGET FFTConvolverLib)
    install(TARGETS FFTConvolverLib RUNTIME DESTINATION ".")
endif()

if(TARGET Vst3HostLib)
    install(TARGETS Vst3HostLib RUNTIME DESTINATION ".")
endif()

if(TARGET AudioEngineLib)
    install(TARGETS AudioEngineLib RUNTIME DESTINATION ".")
endif()

if(TARGET CoreLib)
    install(TARGETS CoreLib RUNTIME DESTINATION ".")
endif()

if(TARGET TranspilersLib)
    install(TARGETS TranspilersLib RUNTIME DESTINATION ".")
endif()

if(TARGET NodeMapLib)
    install(TARGETS NodeMapLib RUNTIME DESTINATION ".")
endif()

if(TARGET UiDesignerLib)
    install(TARGETS UiDesignerLib RUNTIME DESTINATION ".")
endif()

if(TARGET UiComponentsLib)
    install(TARGETS UiComponentsLib RUNTIME DESTINATION ".")
endif()

if(TARGET MapperLib)
    install(TARGETS MapperLib RUNTIME DESTINATION ".")
endif()

if(TARGET MixerLib)
    install(TARGETS MixerLib RUNTIME DESTINATION ".")
endif()

add_executable(test_loader tests/test_loader.cpp)
target_link_libraries(test_loader PRIVATE Qt6::Core CoreLib)


file(TO_CMAKE_PATH "${CMAKE_PREFIX_PATH}/bin/windeployqt.exe" QT_DEPLOY_CMD)

# Deploy Qt and clean up unwanted rtmidi/concurrentqueue install folders
install(CODE "
    include(GetPrerequisites)
    
    # Qt Deployment
    if (EXISTS \"${QT_DEPLOY_CMD}\")
        execute_process(COMMAND \"${QT_DEPLOY_CMD}\" --no-translations --compiler-runtime \"\${CMAKE_INSTALL_PREFIX}/DecentSamplerEditor.exe\")
    else()
        message(WARNING \"windeployqt not found at ${QT_DEPLOY_CMD}\")
    endif()

    # Clean up unwanted development files from dependencies
    file(REMOVE_RECURSE \"\${CMAKE_INSTALL_PREFIX}/include\")
    file(REMOVE_RECURSE \"\${CMAKE_INSTALL_PREFIX}/lib\")
    file(REMOVE_RECURSE \"\${CMAKE_INSTALL_PREFIX}/share\")
    file(REMOVE \"\${CMAKE_INSTALL_PREFIX}/librtmidi.a\")
    file(REMOVE \"\${CMAKE_INSTALL_PREFIX}/librtmidi.dll.a\")
    file(REMOVE \"\${CMAKE_INSTALL_PREFIX}/miniz.a\")
    file(REMOVE \"\${CMAKE_INSTALL_PREFIX}/miniz.dll.a\")
    file(REMOVE \"\${CMAKE_INSTALL_PREFIX}/miniz.lib\")
")
