# cmake/FindCompiler.cmake
# —————————————————————————————————————————————————————————————
# Detectar y configurar el compilador C++
# Si CMAKE_CXX_COMPILER no está definido, busca clang++ o g++
# y fuerza su uso en la caché de CMake.
# —————————————————————————————————————————————————————————————

if(NOT DEFINED CMAKE_CXX_COMPILER)
  find_program(CLANG_COMPILER clang++)
  if(CLANG_COMPILER)
    message(STATUS "Encontrado clang++: ${CLANG_COMPILER}")
    set(CMAKE_CXX_COMPILER "${CLANG_COMPILER}" CACHE STRING "C++ compiler" FORCE)
  else()
    find_program(GNU_COMPILER g++)
    if(GNU_COMPILER)
      message(STATUS "Encontrado g++: ${GNU_COMPILER}")
      set(CMAKE_CXX_COMPILER "${GNU_COMPILER}" CACHE STRING "C++ compiler" FORCE)
    else()
      message(FATAL_ERROR "No se encontró un compilador compatible (clang++ o g++).")
    endif()
  endif()
endif()
