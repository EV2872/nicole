#include "../inc/lexicalAnalysis/nicoleSintax.h"
#include <catch2/catch_test_macros.hpp>
/*
TEST_CASE("The lexer works", "[Lexer]") {
  // Instancia de NicoleSintax
  const nicole::NicoleSintax sintax;
  // Crear el Lexer
  const nicole::Lexer lexer{sintax.createLexer()};
  // Verificar que las categorías del Lexer no estén vacías
  REQUIRE_FALSE(lexer.categories().empty());
  // Ruta al archivo fuente de prueba
  std::filesystem::path testFile{"../examples/literals/literals.nc"};
  // Analizar el archivo fuente
  std::expected<nicole::TokenStream, nicole::Error> result{
      lexer.analyze(testFile)};
  // Verificar que el análisis fue exitoso
  REQUIRE(result.has_value());
  // Verificar que el TokenStream no esté vacío
  REQUIRE(result->size() == 99);

  // Ruta al archivo fuente de prueba
  testFile = std::filesystem::path{"../examples/loops/matrix.nc"};
  // Analizar el archivo fuente
  result = lexer.analyze(testFile);
  // Verificar que el análisis fue exitoso
  REQUIRE(result.has_value());
  // Verificar que el TokenStream no esté vacío
  REQUIRE(result->size() == 84);

  // Ruta al archivo fuente de prueba
  testFile = std::filesystem::path{"../examples/conditionals/conditionals.nc"};
  // Analizar el archivo fuente
  result = lexer.analyze(testFile);
  // Verificar que el análisis fue exitoso
  REQUIRE(result.has_value());
  // Verificar que el TokenStream no esté vacío
  REQUIRE(result->size() == 147);

  // Ruta al archivo fuente de prueba
  testFile = std::filesystem::path{"../examples/conditionals/switch.nc"};
  // Analizar el archivo fuente
  result = lexer.analyze(testFile);
  // Verificar que el análisis fue exitoso
  REQUIRE(result.has_value());
  // Verificar que el TokenStream no esté vacío
  REQUIRE(result->size() == 78);

  // Ruta al archivo fuente de prueba
  testFile = std::filesystem::path{"../examples/functions/functions.nc"};
  // Analizar el archivo fuente
  result = lexer.analyze(testFile);
  // Verificar que el análisis fue exitoso
  REQUIRE(result.has_value());
  // Verificar que el TokenStream no esté vacío
  REQUIRE(result->size() == 172);

  // Ruta al archivo fuente de prueba
  testFile = std::filesystem::path{"../examples/imports/import1.nc"};
  // Analizar el archivo fuente
  result = lexer.analyze(testFile);
  // Verificar que el análisis fue exitoso
  REQUIRE(result.has_value());
  // Verificar que el TokenStream no esté vacío
  REQUIRE(result->size() == 25);

  // Ruta al archivo fuente de prueba
  testFile = std::filesystem::path{"../examples/operations/opWithVariables.nc"};
  // Analizar el archivo fuente
  result = lexer.analyze(testFile);
  // Verificar que el análisis fue exitoso
  REQUIRE(result.has_value());
  // Verificar que el TokenStream no esté vacío
  REQUIRE(result->size() == 202);

  // Ruta al archivo fuente de prueba
  testFile = std::filesystem::path{"../examples/userTypes/classes.nc"};
  // Analizar el archivo fuente
  result = lexer.analyze(testFile);
  // Verificar que el análisis fue exitoso
  REQUIRE(result.has_value());
  // Verificar que el TokenStream no esté vacío
  REQUIRE(result->size() == 60);

  // Ruta al archivo fuente de prueba
  testFile = std::filesystem::path{"../examples/hsufhuhdf"};
  // Analizar el archivo fuente
  result = lexer.analyze(testFile);
  // Verificar que el análisis fue exitoso
  REQUIRE(result.error().type() == nicole::ERROR_TYPE::FILE_EXTENSION);

  // Ruta al archivo fuente de prueba
  testFile = std::filesystem::path{"../examples/unmatched/unmatched"};
  // Analizar el archivo fuente
  result = lexer.analyze(testFile);
  // Verificar que el análisis fue exitoso
  REQUIRE(result.error().type() == nicole::ERROR_TYPE::FILE_EXTENSION);

  // Ruta al archivo fuente de prueba
  testFile = std::filesystem::path{"../examples/unmatched/unmatched.nc"};
  // Analizar el archivo fuente
  result = lexer.analyze(testFile);
  // Verificar que el análisis fue exitoso
  REQUIRE(result.error().type() == nicole::ERROR_TYPE::CHECK_UNMATCHED);
}
*/