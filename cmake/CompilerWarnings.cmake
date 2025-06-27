# cmake/CompilerWarnings.cmake
# —————————————————————————————————————————————————————————————
# Configuración de diagnostics coloreados y flags de warnings
# —————————————————————————————————————————————————————————————

include(CheckCXXCompilerFlag)

# 2) Lista de flags “prefijados” que queremos intentar usar
set(DESIRED_WARNINGS
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wconversion
    -Wsign-conversion
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    -Wmisleading-indentation

    # flags para “no-error”
    -Wno-error=implicit-int-conversion
    -Wno-error=old-style-cast
    -Wno-error=sign-conversion
)

# 3) Variable donde acumularemos sólo los flags soportados
set(SUPPORTED_WARNINGS "")

if (ENABLE_WARNINGS)
  foreach(flag IN LISTS DESIRED_WARNINGS)
    # CMake borra automáticamente los prefijos -Wno-error= etc. para el test,
    # así que lo que chequea es "gcc -Wall", "gcc -Werror", "gcc -Wno-error=foo", etc.
    check_cxx_compiler_flag("${flag}" flag_supported)
    if (flag_supported)
      list(APPEND SUPPORTED_WARNINGS "${flag}")
    else()
      message(STATUS "Descarto flag no soportado: ${flag}")
    endif()
  endforeach()
endif()

# 4) Exponemos la lista de flags resultante
string(REPLACE ";" " " COMPILER_WARNINGS "${SUPPORTED_WARNINGS}")
set(COMPILER_WARNINGS
    "${COMPILER_WARNINGS}"
    CACHE INTERNAL
    "Flags de warnings efectivamente usados por el compilador")
