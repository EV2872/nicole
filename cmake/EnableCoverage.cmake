# cmake/EnableCoverage.cmake
# —————————————————————————————————————————————————————————————
# Habilitar coverage para tests si se activa la opción ENABLE_COVERAGE
# —————————————————————————————————————————————————————————————

if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(STATUS "Coverage habilitado con GCC")
        target_compile_options(tests PRIVATE --coverage)
        target_link_options(tests PRIVATE --coverage)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(STATUS "Coverage habilitado con Clang")
        target_compile_options(tests PRIVATE -fprofile-instr-generate -fcoverage-mapping)
        target_link_options(tests PRIVATE -fprofile-instr-generate -fcoverage-mapping)
    else()
        message(WARNING "Coverage solo soportado para GCC o Clang")
    endif()
endif()