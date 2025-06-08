# Minimum CMake version
cmake_minimum_required(VERSION 3.30.5)

# usar en la linea de comandos para generar compile_commands.json y que .clangd detecte catch2
# cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .

# Detectar y configurar el compilador
if(NOT DEFINED CMAKE_CXX_COMPILER)
    find_program(CLANG_COMPILER clang++)
    if(CLANG_COMPILER)
        set(CMAKE_CXX_COMPILER "${CLANG_COMPILER}" CACHE STRING "C++ compiler" FORCE)
    else()
        find_program(GNU_COMPILER g++)
        if(GNU_COMPILER)
            set(CMAKE_CXX_COMPILER "${GNU_COMPILER}" CACHE STRING "C++ compiler" FORCE)
        else()
            message(FATAL_ERROR "No se encontró un compilador compatible (clang++ o g++).")
        endif()
    endif()
endif()

project(MyProject LANGUAGES CXX)

# Configurar ccache (si está instalado)
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Usando ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

# C++ standard y optimizaciones
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Activate LTO optimization
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Colored erros
set(CMAKE_COLOR_DIAGNOSTICS ON)

# Configurar warnings
option(ENABLE_WARNINGS "Activar flags de warnings" ON)
if(ENABLE_WARNINGS)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(COMPILER_WARNINGS "-Wall -Wextra -Werror -Wpedantic \
                               -Wshadow -Wnon-virtual-dtor -Wold-style-cast \
                               -Wcast-align -Wunused -Woverloaded-virtual \
                               -Wconversion -Wsign-conversion -Wnull-dereference \
                               -Wdouble-promotion -Wformat=2 -Wmisleading-indentation")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(COMPILER_WARNINGS "-Wall -Wextra -Werror -Wpedantic \
                               -Wshadow -Wnon-virtual-dtor -Wold-style-cast \
                               -Wcast-align -Wunused -Woverloaded-virtual \
                               -Wconversion -Wsign-conversion -Wnull-dereference \
                               -Wdouble-promotion -Wformat=2 -Wmisleading-indentation \
                               -Wduplicated-cond -Wduplicated-branches -Wlogical-op \
                               -Wuseless-cast")
    else()
        message(WARNING "No se aplicaron flags específicos para el compilador.")
    endif()
else()
    set(COMPILER_WARNINGS "")
endif()

# Directorios de salida
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

###########################################################
# catch2
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.7.0
)
FetchContent_MakeAvailable(Catch2)

add_executable(tests
    # Lista de archivos para tests
    test/errors/test_error.cc
    test/options/test_options.cc
    test/options/test_optionsParser.cc
    src/options/optionsParser.cc
    # lexicalAnalysis
    test/lexicalAnalysis/test_category.cc
    test/lexicalAnalysis/test_lexer.cc
    src/lexicalAnalysis/lexer.cc
    src/lexicalAnalysis/type.cc
    test/lexicalAnalysis/test_location.cc
    test/lexicalAnalysis/test_regexWrapper.cc
    test/lexicalAnalysis/test_sintax.cc
    src/lexicalAnalysis/nicoleSintax.cc
    test/lexicalAnalysis/test_token.cc
    test/lexicalAnalysis/test_tokenStream.cc
    src/lexicalAnalysis/tokenStream.cc
    test/lexicalAnalysis/test_type.cc
    # parsingAnalysis
    src/parsingAnalysis/ast/astType.cc 
    src/parsingAnalysis/builder.cc
    src/parsingAnalysis/checkPosition.cc
    src/parsingAnalysis/algorithm/topDown.cc
    src/parsingAnalysis/algorithm/productions/parseBinary.cc
    src/parsingAnalysis/algorithm/productions/parseConditionals.cc
    src/parsingAnalysis/algorithm/productions/parseDecl.cc
    src/parsingAnalysis/algorithm/productions/parseFactor.cc
    src/parsingAnalysis/algorithm/productions/parseFunctions.cc
    src/parsingAnalysis/algorithm/productions/parseLoops.cc
    src/parsingAnalysis/algorithm/productions/parseSelfAssignment.cc
    src/parsingAnalysis/algorithm/productions/parseUserTypes.cc
    src/parsingAnalysis/algorithm/productions/parseUtils.cc
    src/parsingAnalysis/algorithm/productions/parseChained.cc
    src/parsingAnalysis/algorithm/productions/parseTypes.cc
    test/parsingAnalysis/test_ast_literals.cc
    test/parsingAnalysis/test_ast_vector.cc 
    test/parsingAnalysis/test_ast_variables.cc 
    test/parsingAnalysis/test_ast_utils.cc 
    test/parsingAnalysis/test_ast_statements.cc 
    test/parsingAnalysis/test_ast_assignment.cc 
    test/parsingAnalysis/test_ast_unary.cc 
    test/parsingAnalysis/test_ast_binary.cc
    test/parsingAnalysis/test_ast_conditionals.cc
    test/parsingAnalysis/test_ast_loops.cc
    test/parsingAnalysis/test_ast_functions.cc
    test/parsingAnalysis/test_ast_pointers.cc

    # tables
    src/tables/functionTable/functionTable.cc

    src/tables/typeTable/typeTable.cc
    src/tables/typeTable/typeManager.cc
    src/tables/typeTable/userType.cc
    src/tables/typeTable/methodTable.cc
    src/tables/typeTable/attrTable.cc
    src/tables/scope/scope.cc

    # visitors
    src/visitors/printTree/printTree.cc
    src/visitors/printTree/printTreeAssignment.cc
    src/visitors/printTree/printTreeChained.cc
    src/visitors/printTree/printTreeConditionals.cc
    src/visitors/printTree/printTreeEnum.cc
    src/visitors/printTree/printTreeFactor.cc
    src/visitors/printTree/printTreeFunctions.cc
    src/visitors/printTree/printTreeLoops.cc
    src/visitors/printTree/printTreeOperations.cc
    src/visitors/printTree/printTreePointers.cc
    src/visitors/printTree/printTreeUserTypes.cc
    src/visitors/printTree/printTreeUtils.cc
    src/visitors/printTree/printTreeVarDecl.cc
    src/visitors/printTree/printTreeVector.cc

    ######
    src/visitors/validateTree/validateTree.cc
    src/visitors/validateTree/validateAssignment.cc
    src/visitors/validateTree/validateChained.cc
    src/visitors/validateTree/validateConditionals.cc
    src/visitors/validateTree/validateEnum.cc
    src/visitors/validateTree/validateFactor.cc
    src/visitors/validateTree/validateFunctions.cc
    src/visitors/validateTree/validateLoops.cc
    src/visitors/validateTree/validateOperations.cc
    src/visitors/validateTree/validatePointers.cc
    src/visitors/validateTree/validateUserTypes.cc
    src/visitors/validateTree/validateUtils.cc
    src/visitors/validateTree/validateVarDecl.cc
    src/visitors/validateTree/validateVector.cc
    
    #####
    src/visitors/fillSemanticInfo/fillSemanticInfo.cc
    src/visitors/fillSemanticInfo/fillSemanticAssignment.cc
    src/visitors/fillSemanticInfo/fillSemanticChained.cc
    src/visitors/fillSemanticInfo/fillSemanticConditionals.cc
    src/visitors/fillSemanticInfo/fillSemanticEnum.cc
    src/visitors/fillSemanticInfo/fillSemanticFactor.cc
    src/visitors/fillSemanticInfo/fillSemanticFunctions.cc
    src/visitors/fillSemanticInfo/fillSemanticLoops.cc
    src/visitors/fillSemanticInfo/fillSemanticOperations.cc
    src/visitors/fillSemanticInfo/fillSemanticPointers.cc
    src/visitors/fillSemanticInfo/fillSemanticUserTypes.cc
    src/visitors/fillSemanticInfo/fillSemanticUtils.cc
    src/visitors/fillSemanticInfo/fillSemanticVarDecl.cc
    src/visitors/fillSemanticInfo/fillSemanticVector.cc

    #####
    src/visitors/monomorphize/monomorphize.cc
    src/visitors/monomorphize/monomorphizeAssignment.cc
    src/visitors/monomorphize/monomorphizeChained.cc
    src/visitors/monomorphize/monomorphizeConditionals.cc
    src/visitors/monomorphize/monomorphizeEnum.cc
    src/visitors/monomorphize/monomorphizeFactor.cc
    src/visitors/monomorphize/monomorphizeFunctions.cc
    src/visitors/monomorphize/monomorphizeLoops.cc
    src/visitors/monomorphize/monomorphizeOperations.cc
    src/visitors/monomorphize/monomorphizePointers.cc
    src/visitors/monomorphize/monomorphizeUserTypes.cc
    src/visitors/monomorphize/monomorphizeUtils.cc
    src/visitors/monomorphize/monomorphizeVarDecl.cc
    src/visitors/monomorphize/monomorphizeVector.cc
    
    #####
    src/visitors/typeAnalysis/typeAnalysis.cc
    src/visitors/typeAnalysis/typeAnalysisAssignment.cc
    src/visitors/typeAnalysis/typeAnalysisChained.cc
    src/visitors/typeAnalysis/typeAnalysisConditionals.cc
    src/visitors/typeAnalysis/typeAnalysisEnum.cc
    src/visitors/typeAnalysis/typeAnalysisFactor.cc
    src/visitors/typeAnalysis/typeAnalysisFunctions.cc
    src/visitors/typeAnalysis/typeAnalysisLoops.cc
    src/visitors/typeAnalysis/typeAnalysisOperations.cc
    src/visitors/typeAnalysis/typeAnalysisPointers.cc
    src/visitors/typeAnalysis/typeAnalysisUserTypes.cc
    src/visitors/typeAnalysis/typeAnalysisUtils.cc
    src/visitors/typeAnalysis/typeAnalysisVarDecl.cc
    src/visitors/typeAnalysis/typeAnalysisVector.cc
    
    #####
    src/visitors/codeGeneration/codeGeneration.cc
    src/visitors/codeGeneration/codeGenAssignment.cc
    src/visitors/codeGeneration/codeGenChained.cc
    src/visitors/codeGeneration/codeGenConditionals.cc
    src/visitors/codeGeneration/codeGenEnum.cc
    src/visitors/codeGeneration/codeGenFactor.cc
    src/visitors/codeGeneration/codeGenFunctions.cc
    src/visitors/codeGeneration/codeGenLoops.cc
    src/visitors/codeGeneration/codeGenOperations.cc
    src/visitors/codeGeneration/codeGenPointers.cc
    src/visitors/codeGeneration/codeGenUserTypes.cc
    src/visitors/codeGeneration/codeGenUtils.cc
    src/visitors/codeGeneration/codeGenVarDecl.cc
    src/visitors/codeGeneration/codeGenVector.cc

    # compiler
    src/compiler/nicole.cc

    # utils
    src/utils/nameMangler.cc
)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_compile_options(tests PRIVATE
    -Wno-error=implicit-int-conversion
    -Wno-error=old-style-cast
    -Wno-error=sign-conversion
)

# Habilitar coverage para tests si se activa la opción ENABLE_COVERAGE
option(ENABLE_COVERAGE "Habilitar flags de coverage para tests" OFF)
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

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests)

###########################################################
# Configuración de LLVM
option(USE_LLVM_CONFIG_19 "Utilizar llvm-config-19 en lugar de llvm-config" OFF)
if(USE_LLVM_CONFIG_19)
    set(LLVM_CONFIG "llvm-config-19")
else()
    set(LLVM_CONFIG "llvm-config")
endif()

execute_process(
    COMMAND  ${LLVM_CONFIG} --cxxflags
    OUTPUT_VARIABLE LLVM_CXX_FLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND  ${LLVM_CONFIG} --ldflags
    OUTPUT_VARIABLE LLVM_LDFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND  ${LLVM_CONFIG} --system-libs
    OUTPUT_VARIABLE LLVM_SYSTEM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND  ${LLVM_CONFIG} --libs core
    OUTPUT_VARIABLE LLVM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND  ${LLVM_CONFIG} --includedir
    OUTPUT_VARIABLE LLVM_INCLUDEDIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

include_directories(${LLVM_INCLUDEDIR})

# Set compiler and linker flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 ${COMPILER_WARNINGS} ${LLVM_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LLVM_LDFLAGS} ${LLVM_SYSTEM_LIBS} ${LLVM_LIBS}")

# Source files para el ejecutable principal
set(SRC_FILES
    src/main.cc
    src/options/optionsParser.cc
    src/lexicalAnalysis/type.cc
    src/lexicalAnalysis/tokenStream.cc
    src/lexicalAnalysis/lexer.cc
    src/lexicalAnalysis/nicoleSintax.cc
    src/parsingAnalysis/ast/astType.cc
    src/parsingAnalysis/builder.cc
    src/parsingAnalysis/checkPosition.cc
    src/parsingAnalysis/algorithm/topDown.cc
    src/parsingAnalysis/algorithm/productions/parseBinary.cc
    src/parsingAnalysis/algorithm/productions/parseConditionals.cc
    src/parsingAnalysis/algorithm/productions/parseDecl.cc
    src/parsingAnalysis/algorithm/productions/parseFactor.cc
    src/parsingAnalysis/algorithm/productions/parseFunctions.cc
    src/parsingAnalysis/algorithm/productions/parseLoops.cc
    src/parsingAnalysis/algorithm/productions/parseSelfAssignment.cc
    src/parsingAnalysis/algorithm/productions/parseUserTypes.cc
    src/parsingAnalysis/algorithm/productions/parseUtils.cc
    src/parsingAnalysis/algorithm/productions/parseChained.cc
    src/parsingAnalysis/algorithm/productions/parseTypes.cc

    src/tables/functionTable/functionTable.cc
    src/tables/typeTable/typeTable.cc
    src/tables/typeTable/typeManager.cc
    src/tables/typeTable/userType.cc
    src/tables/typeTable/methodTable.cc
    src/tables/typeTable/attrTable.cc
    src/tables/scope/scope.cc

    # visitors
    src/visitors/printTree/printTree.cc
    src/visitors/printTree/printTreeAssignment.cc
    src/visitors/printTree/printTreeChained.cc
    src/visitors/printTree/printTreeConditionals.cc
    src/visitors/printTree/printTreeEnum.cc
    src/visitors/printTree/printTreeFactor.cc
    src/visitors/printTree/printTreeFunctions.cc
    src/visitors/printTree/printTreeLoops.cc
    src/visitors/printTree/printTreeOperations.cc
    src/visitors/printTree/printTreePointers.cc
    src/visitors/printTree/printTreeUserTypes.cc
    src/visitors/printTree/printTreeUtils.cc
    src/visitors/printTree/printTreeVarDecl.cc
    src/visitors/printTree/printTreeVector.cc

    ######
    src/visitors/validateTree/validateTree.cc
    src/visitors/validateTree/validateAssignment.cc
    src/visitors/validateTree/validateChained.cc
    src/visitors/validateTree/validateConditionals.cc
    src/visitors/validateTree/validateEnum.cc
    src/visitors/validateTree/validateFactor.cc
    src/visitors/validateTree/validateFunctions.cc
    src/visitors/validateTree/validateLoops.cc
    src/visitors/validateTree/validateOperations.cc
    src/visitors/validateTree/validatePointers.cc
    src/visitors/validateTree/validateUserTypes.cc
    src/visitors/validateTree/validateUtils.cc
    src/visitors/validateTree/validateVarDecl.cc
    src/visitors/validateTree/validateVector.cc
    
    #####
    src/visitors/fillSemanticInfo/fillSemanticInfo.cc
    src/visitors/fillSemanticInfo/fillSemanticAssignment.cc
    src/visitors/fillSemanticInfo/fillSemanticChained.cc
    src/visitors/fillSemanticInfo/fillSemanticConditionals.cc
    src/visitors/fillSemanticInfo/fillSemanticEnum.cc
    src/visitors/fillSemanticInfo/fillSemanticFactor.cc
    src/visitors/fillSemanticInfo/fillSemanticFunctions.cc
    src/visitors/fillSemanticInfo/fillSemanticLoops.cc
    src/visitors/fillSemanticInfo/fillSemanticOperations.cc
    src/visitors/fillSemanticInfo/fillSemanticPointers.cc
    src/visitors/fillSemanticInfo/fillSemanticUserTypes.cc
    src/visitors/fillSemanticInfo/fillSemanticUtils.cc
    src/visitors/fillSemanticInfo/fillSemanticVarDecl.cc
    src/visitors/fillSemanticInfo/fillSemanticVector.cc

    #####
    src/visitors/monomorphize/monomorphize.cc
    src/visitors/monomorphize/monomorphizeAssignment.cc
    src/visitors/monomorphize/monomorphizeChained.cc
    src/visitors/monomorphize/monomorphizeConditionals.cc
    src/visitors/monomorphize/monomorphizeEnum.cc
    src/visitors/monomorphize/monomorphizeFactor.cc
    src/visitors/monomorphize/monomorphizeFunctions.cc
    src/visitors/monomorphize/monomorphizeLoops.cc
    src/visitors/monomorphize/monomorphizeOperations.cc
    src/visitors/monomorphize/monomorphizePointers.cc
    src/visitors/monomorphize/monomorphizeUserTypes.cc
    src/visitors/monomorphize/monomorphizeUtils.cc
    src/visitors/monomorphize/monomorphizeVarDecl.cc
    src/visitors/monomorphize/monomorphizeVector.cc
    
    #####
    src/visitors/typeAnalysis/typeAnalysis.cc
    src/visitors/typeAnalysis/typeAnalysisAssignment.cc
    src/visitors/typeAnalysis/typeAnalysisChained.cc
    src/visitors/typeAnalysis/typeAnalysisConditionals.cc
    src/visitors/typeAnalysis/typeAnalysisEnum.cc
    src/visitors/typeAnalysis/typeAnalysisFactor.cc
    src/visitors/typeAnalysis/typeAnalysisFunctions.cc
    src/visitors/typeAnalysis/typeAnalysisLoops.cc
    src/visitors/typeAnalysis/typeAnalysisOperations.cc
    src/visitors/typeAnalysis/typeAnalysisPointers.cc
    src/visitors/typeAnalysis/typeAnalysisUserTypes.cc
    src/visitors/typeAnalysis/typeAnalysisUtils.cc
    src/visitors/typeAnalysis/typeAnalysisVarDecl.cc
    src/visitors/typeAnalysis/typeAnalysisVector.cc
    
    #####
    src/visitors/codeGeneration/codeGeneration.cc
    src/visitors/codeGeneration/codeGenAssignment.cc
    src/visitors/codeGeneration/codeGenChained.cc
    src/visitors/codeGeneration/codeGenConditionals.cc
    src/visitors/codeGeneration/codeGenEnum.cc
    src/visitors/codeGeneration/codeGenFactor.cc
    src/visitors/codeGeneration/codeGenFunctions.cc
    src/visitors/codeGeneration/codeGenLoops.cc
    src/visitors/codeGeneration/codeGenOperations.cc
    src/visitors/codeGeneration/codeGenPointers.cc
    src/visitors/codeGeneration/codeGenUserTypes.cc
    src/visitors/codeGeneration/codeGenUtils.cc
    src/visitors/codeGeneration/codeGenVarDecl.cc
    src/visitors/codeGeneration/codeGenVector.cc

    src/compiler/nicole.cc

    # utils
    src/utils/nameMangler.cc
)

# Include directories para el ejecutable principal
include_directories(${CMAKE_SOURCE_DIR}/src)

# Executable principal
add_executable(binaries ${SRC_FILES})
target_include_directories(binaries PRIVATE ${LLVM_INCLUDEDIR})
target_link_libraries(binaries ${LLVM_LIBS} ${LLVM_SYSTEM_LIBS})

# Custom targets
add_custom_target(run
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/binaries
    DEPENDS binaries
    COMMENT "Running the program..."
)
add_custom_target(ignore
    COMMAND git add .gitignore
    COMMENT "Adding .gitignore file to git..."
)
add_custom_target(cln
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/bin/obj
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/bin
    COMMENT "Cleaning up..."
)
