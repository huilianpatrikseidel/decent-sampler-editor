import re

with open('CMakeLists.txt', 'r', encoding='utf-8') as f:
    content = f.read()

targets = [
    "AudioEngineLib", "CoreLib", "TranspilersLib", "NodeMapLib", 
    "UiDesignerLib", "UiComponentsLib", "MapperLib", "MixerLib",
    "MiniaudioLib", "FFTConvolverLib", "Vst3HostLib"
]

for target in targets:
    # Find add_library(TargetName ...) and insert ${CMAKE_CURRENT_BINARY_DIR}/TargetName.rc inside the parenthesis
    # We will just append it before the closing parenthesis of add_library.
    # It might be multiline.
    pattern = re.compile(rf'(add_library\({target}(?:\s+SHARED|\s+STATIC)?\s+.*?)\)', re.DOTALL)
    
    def repl(m):
        return f'{m.group(1)}\n    ${{CMAKE_CURRENT_BINARY_DIR}}/{target}.rc\n)'
    
    content, count = pattern.subn(repl, content, count=1)
    if count == 0:
        print(f"Failed to patch {target}")
        continue
        
    # Also add add_dependencies(target UpdateBuildNumber) right after add_library or set_target_properties
    # We'll insert it right after the add_library(... ) block.
    # We find the end of the add_library block
    add_lib_pattern = re.compile(rf'(add_library\({target}.*?\))', re.DOTALL)
    
    def add_dep_repl(m):
        return f'{m.group(1)}\nadd_dependencies({target} UpdateBuildNumber)'
    
    content = add_lib_pattern.sub(add_dep_repl, content, count=1)

with open('CMakeLists.txt', 'w', encoding='utf-8') as f:
    f.write(content)

print("Done")
