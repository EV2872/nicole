# cmake/FindCppcheck.cmake
# —————————————————————————————————————————————————————————————
# Configura cppcheck
# —————————————————————————————————————————————————————————————

find_program(CPPCHECK_EXE NAMES cppcheck)
if(CPPCHECK_EXE)
  add_custom_target(cppcheck
    COMMAND env CLICOLOR_FORCE=1 ${CPPCHECK_EXE}
      --project=${CMAKE_BINARY_DIR}/compile_commands.json
      --suppressions-list=${CMAKE_SOURCE_DIR}/cppcheck-suppress.txt
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Ejecutando cppcheck solo en src/ con color"
    USES_TERMINAL
  )
endif()
