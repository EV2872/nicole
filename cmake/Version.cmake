# cmake/Version.cmake
# —————————————————————————————————————————————————————————————
# Obtiene la versión de git y genera version.h a partir de version.h.in
# —————————————————————————————————————————————————————————————

# Asegúrate de que exista el directorio de salida
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/build/generate")

# Obtener la versión desde git
execute_process(
  COMMAND git describe --tags --always
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Configurar version.h
configure_file(
  "${CMAKE_SOURCE_DIR}/version/version.h.in"
  "${CMAKE_BINARY_DIR}/build/generate/version.h"
  @ONLY
)

# Exponer la ruta de headers generados para que otros targets la incluyan
set(VERSION_HEADER_DIR "${CMAKE_BINARY_DIR}/build/generate"
    CACHE INTERNAL "Directorio de headers generados por Version.cmake")
