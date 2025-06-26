# Minimum CMake version
cmake_minimum_required(VERSION 3.31.6)

set_property(DIRECTORY
             APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
             "${CMAKE_SOURCE_DIR}/config.json")


# 1. Leer el contenido del fichero
file(READ "${CMAKE_SOURCE_DIR}/config.json" _config_json)

string(JSON PROJECT_NAME_JSON GET "${_config_json}" project name)
# usar en la linea de comandos para generar compile_commands.json y que .clangd detecte catch2
# cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .

# Añadir nuestra carpeta de módulos personalizados
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_SOURCE_DIR}/cmake"
)

function(load_modules)
    foreach(m IN LISTS ARGN)
        include(${m})
    endforeach()
endfunction()

#[[ Cargar módulos internos
load_modules(

)
]]

# Incluir el módulo que detecta y configura el compilador
include(FindCompiler)

# Incluir nuestro módulo de versión
include(Version)

# Añadir al include path
include_directories(${VERSION_HEADER_DIR})

project(${PROJECT_NAME_JSON} LANGUAGES CXX)

# Incluir el módulo que detecta y configura el ccache
include(FindCcache)

# C++ standard y optimizaciones
include(CplusplusStandarAndLTO)

# warnings de clang y gcc
include(CompilerWarnings)

# Directorios de salida
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

###########################################################
# catch2
include(FetchCatch2)

# Incluir los módulos de fuentes
include(FindCommonSourceFiles)
include(FindTestSourceFiles)

add_library(Catch2System INTERFACE)
target_include_directories(
  Catch2System
  SYSTEM INTERFACE
    ${_catch2_incs}
)


add_executable(tests
    ${COMMON_SOURCES}
    ${TEST_SOURCES}
)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_compile_options(tests PRIVATE
    -Wno-error=implicit-int-conversion
    -Wno-error=old-style-cast
    -Wno-error=sign-conversion
)

# enables coverage for tests
include(EnableCoverage)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests)

###########################################################
include(ConfigureLLVM)
include_directories(SYSTEM ${LLVM_INCLUDEDIR})

# Set compiler and linker flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 ${COMPILER_WARNINGS} ${LLVM_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LLVM_LDFLAGS} ${LLVM_SYSTEM_LIBS} ${LLVM_LIBS}")

# Include directories para el ejecutable principal
include_directories(${CMAKE_SOURCE_DIR}/src)

# Incluir el módulo que detecta y configura el clang tidy
include(FindClangTidy)


# === export compile_commands.json ===
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Incluir el módulo que detecta y configura el cppcheck
include(FindCppcheck)

# Executable principal
add_executable(binaries ${COMMON_SOURCES} src/main.cc)
target_include_directories(binaries PRIVATE ${LLVM_INCLUDEDIR})
target_link_libraries(binaries ${LLVM_LIBS} ${LLVM_SYSTEM_LIBS})
