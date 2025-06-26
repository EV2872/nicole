# cmake/FindClangTidy.cmake
# —————————————————————————————————————————————————————————————
# Configura clang-tidy
# —————————————————————————————————————————————————————————————

#find_program(CLANG_TIDY_EXE NAMES clang-tidy)
#if(CLANG_TIDY_EXE)
#  set(CMAKE_CXX_CLANG_TIDY
#    "${CLANG_TIDY_EXE};
#     -checks=*;
#     -header-filter=^${CMAKE_SOURCE_DIR}/src/;
#     -warnings-as-errors=*;
#     --use-color")
#  message(STATUS "clang-tidy habilitado (solo src/)")
#endif()