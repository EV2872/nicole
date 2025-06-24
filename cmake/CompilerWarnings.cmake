# cmake/CompilerWarnings.cmake
# —————————————————————————————————————————————————————————————
# Configuración de diagnostics coloreados y flags de warnings
# —————————————————————————————————————————————————————————————

# Habilitar diagnósticos coloreados en CMake 3.14+
set(CMAKE_COLOR_DIAGNOSTICS ON)

# Opción para activar o desactivar warnings
option(ENABLE_WARNINGS "Activar flags de warnings" ON)

if(ENABLE_WARNINGS)
  # Flags comunes a Clang y GCC
  set(COMMON_WARNINGS
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
  )

  # Añadir flags específicos
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(SPECIFIC_WARNINGS
        # (ningún flag adicional por ahora)
    )
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(SPECIFIC_WARNINGS
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op
        -Wuseless-cast
    )
  else()
    message(WARNING "No se aplicaron flags específicos para ${CMAKE_CXX_COMPILER_ID}")
    set(SPECIFIC_WARNINGS "")
  endif()

  # Construir la variable final
  string(REPLACE ";" " " COMPILER_WARNINGS
         "${COMMON_WARNINGS};${SPECIFIC_WARNINGS}")

else()
  set(COMPILER_WARNINGS "")
endif()

# Exponer COMPILER_WARNINGS en la caché para ser usada en el CMakeLists principal
set(COMPILER_WARNINGS "${COMPILER_WARNINGS}" CACHE INTERNAL "Flags de warnings para el compilador")
