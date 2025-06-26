# cmake/FindCcache.cmake
# —————————————————————————————————————————————————————————————
# Configura ccache
# —————————————————————————————————————————————————————————————

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Usando ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()