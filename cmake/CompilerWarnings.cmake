# —————————————————————————————————————————————————————————————
# cmake/CompilerWarnings.cmake
# Detecta warnings soportados y se invalida si cambia el compilador
# —————————————————————————————————————————————————————————————

include(CheckCXXCompilerFlag)

# 1) Nombre de variable donde guardaremos el último compilador usado
set(_CACHE_COMPILER_VAR "CACHED_CXX_COMPILER_ID")

# 2) Obtenemos el ID actual del compilador (e.g. "Clang" o "GNU")
#    Esto funciona porque CMake define CMAKE_CXX_COMPILER_ID
if (NOT DEFINED CMAKE_CXX_COMPILER_ID)
  message(FATAL_ERROR "CMAKE_CXX_COMPILER_ID no está definido. ¿Configuraste CMAKE_CXX_COMPILER?")
endif()
set(_current_compiler "${CMAKE_CXX_COMPILER_ID}")

# 3) Recuperamos el compilador anterior en caché (si existe)
if (DEFINED ${_CACHE_COMPILER_VAR})
  set(_cached_compiler "${${_CACHE_COMPILER_VAR}}")
else()
  set(_cached_compiler "")
endif()

# 4) Si cambió el compilador, limpiamos las variables de flags en caché
if (NOT _current_compiler STREQUAL _cached_compiler)
  message(STATUS "Detectado cambio de compilador: '${_cached_compiler}' → '${_current_compiler}'. Invalidando caché de warnings.")
  # Eliminamos la variable principal de warnings
  unset(COMPILER_WARNINGS CACHE)
  # (Opcional) Eliminamos las variables temporales que CheckCXXCompilerFlag pudo haber cacheado
  # Si usas nombres fijos para cada flag, podrías unsetlos aquí con -U
endif()

# 5) Actualizamos el compilador en caché para la próxima vez
set(${_CACHE_COMPILER_VAR} "${_current_compiler}" CACHE INTERNAL
    "Identificador del compilador CXX detectado en la última configuración")

# 6) Lista de flags “prefijados” que queremos intentar usar
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
#    -Wno-error=implicit-int-conversion
#    -Wno-error=old-style-cast
#    -Wno-error=sign-conversion
)

# 7) Variable donde acumularemos solo los flags soportados
set(SUPPORTED_WARNINGS "")

if (ENABLE_WARNINGS)
  foreach(flag IN LISTS DESIRED_WARNINGS)
    check_cxx_compiler_flag("${flag}" flag_supported)
    if (flag_supported)
      list(APPEND SUPPORTED_WARNINGS "${flag}")
    else()
      message(STATUS "Descarto flag no soportado: ${flag}")
    endif()
  endforeach()
endif()

# 8) Exponemos la lista de flags resultante en caché interna
string(REPLACE ";" " " COMPILER_WARNINGS "${SUPPORTED_WARNINGS}")
set(COMPILER_WARNINGS
    "${COMPILER_WARNINGS}"
    CACHE INTERNAL
    "Flags de warnings efectivamente usados por el compilador ${_current_compiler}"
)
