set(BUILD_NUMBER_FILE "${SRC_DIR}/build_number.txt")
if(EXISTS "${BUILD_NUMBER_FILE}")
    file(READ "${BUILD_NUMBER_FILE}" BUILD_NUMBER)
    string(STRIP "${BUILD_NUMBER}" BUILD_NUMBER)
else()
    set(BUILD_NUMBER 0)
endif()

math(EXPR BUILD_NUMBER "${BUILD_NUMBER} + 1")
file(WRITE "${BUILD_NUMBER_FILE}" "${BUILD_NUMBER}\n")

set(CPP_CONTENT "const char* APP_BUILD_NUMBER = \"${BUILD_NUMBER}\";\n")
set(CPP_FILE "${BIN_DIR}/build_version.cpp")

if(EXISTS "${CPP_FILE}")
    file(READ "${CPP_FILE}" OLD_CPP_CONTENT)
else()
    set(OLD_CPP_CONTENT "")
endif()

if(NOT "${CPP_CONTENT}" STREQUAL "${OLD_CPP_CONTENT}")
    file(WRITE "${CPP_FILE}" "${CPP_CONTENT}")
endif()

# Generate the app-icon.rc with the updated build number
configure_file("${SRC_DIR}/app-icon.rc.in" "${BIN_DIR}/app-icon.rc" @ONLY)

# Generate RC files for DLLs
set(DLL_TARGETS 
    "AudioEngineLib" "CoreLib" "TranspilersLib" "NodeMapLib" 
    "UiDesignerLib" "UiComponentsLib" "MapperLib" "MixerLib"
    "MiniaudioLib" "FFTConvolverLib" "Vst3HostLib"
    "rtmidi" "miniz"
)

foreach(TARGET_NAME IN LISTS DLL_TARGETS)
    set(DLL_PRODUCT_NAME "Decent Sampler Editor")
    set(DLL_COPYRIGHT "Copyright (C) 2026")
    set(DLL_BASENAME "${TARGET_NAME}")
    string(REGEX REPLACE "Lib$" "" COMPONENT_NAME "${TARGET_NAME}")
    set(DLL_FILE_DESCRIPTION "DS ${COMPONENT_NAME} Component")

    if(TARGET_NAME STREQUAL "rtmidi")
        set(DLL_PRODUCT_NAME "RtMidi")
        set(DLL_COPYRIGHT "Copyright (c) 2003-2023 Gary P. Scavone")
        set(DLL_FILE_DESCRIPTION "rtmidi Library")
    elseif(TARGET_NAME STREQUAL "miniz")
        set(DLL_PRODUCT_NAME "miniz")
        set(DLL_COPYRIGHT "Copyright 2013-2014 RAD Game Tools and Valve Software, Copyright 2010-2014 Rich Geldreich")
        set(DLL_FILE_DESCRIPTION "miniz Library")
    elseif(TARGET_NAME STREQUAL "MiniaudioLib")
        set(DLL_PRODUCT_NAME "miniaudio")
        set(DLL_COPYRIGHT "Copyright (c) David Reid")
        set(DLL_BASENAME "miniaudio")
        set(DLL_FILE_DESCRIPTION "miniaudio Library")
    elseif(TARGET_NAME STREQUAL "FFTConvolverLib")
        set(DLL_PRODUCT_NAME "FFTConvolver")
        set(DLL_COPYRIGHT "Copyright (c) 2017 HiFi-LoFi")
        set(DLL_BASENAME "FFTConvolver")
        set(DLL_FILE_DESCRIPTION "FFTConvolver Library")
    elseif(TARGET_NAME STREQUAL "Vst3HostLib")
        set(DLL_PRODUCT_NAME "VST3 SDK")
        set(DLL_COPYRIGHT "Copyright (c) 2024, Steinberg Media Technologies GmbH")
        set(DLL_BASENAME "Vst3Host")
        set(DLL_FILE_DESCRIPTION "Vst3Host Library")
    else()
        # Custom Decent Sampler components
        set(DLL_BASENAME "ds${COMPONENT_NAME}")
    endif()

    configure_file("${SRC_DIR}/src/dll-metadata.rc.in" "${BIN_DIR}/${TARGET_NAME}.rc" @ONLY)
endforeach()
