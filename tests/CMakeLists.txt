# Configuração do binário de testes
add_executable(SamplerEditorTests main_test.cpp)

target_include_directories(SamplerEditorTests PRIVATE ../src)

target_link_libraries(SamplerEditorTests PRIVATE
    SamplerEditorLib
    Qt6::Widgets
    Qt6::Test
)

# Registra os testes no CTest do CMake
add_test(NAME SamplerEditorTests COMMAND SamplerEditorTests)
