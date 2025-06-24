# Source files para el ejecutable test
set(TEST_SOURCES
    test/options/test_options.cc
    test/options/test_optionsParser.cc

    # Tests de errores
    test/errors/test_error.cc

    # Tests de lexicalAnalysis
    test/lexicalAnalysis/test_category.cc
    test/lexicalAnalysis/test_lexer.cc
    test/lexicalAnalysis/test_location.cc
    test/lexicalAnalysis/test_regexWrapper.cc
    test/lexicalAnalysis/test_sintax.cc
    test/lexicalAnalysis/test_token.cc
    test/lexicalAnalysis/test_tokenStream.cc
    test/lexicalAnalysis/test_type.cc

    # Tests de parsingAnalysis
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
)