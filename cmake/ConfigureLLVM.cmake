# cmake/ConfigureLLVM.cmake
# —————————————————————————————————————————————————————————————
# Configuración de backend LLVM
# —————————————————————————————————————————————————————————————
option(USE_LLVM_CONFIG_20 "Utilizar llvm-config-20 en lugar de llvm-config" OFF)
if(USE_LLVM_CONFIG_20)
    set(LLVM_CONFIG "llvm-config-20")
else()
    set(LLVM_CONFIG "llvm-config")
endif()

execute_process(
    COMMAND  ${LLVM_CONFIG} --cxxflags
    OUTPUT_VARIABLE LLVM_CXX_FLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND  ${LLVM_CONFIG} --ldflags
    OUTPUT_VARIABLE LLVM_LDFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND  ${LLVM_CONFIG} --system-libs
    OUTPUT_VARIABLE LLVM_SYSTEM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND  ${LLVM_CONFIG} --libs core
    OUTPUT_VARIABLE LLVM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND  ${LLVM_CONFIG} --includedir
    OUTPUT_VARIABLE LLVM_INCLUDEDIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)