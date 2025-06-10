#include "../inc/options/optionsParser.h"
#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include <vector>

/*
TEST_CASE("Pruebas de OptionsParser con diferentes cantidades de argumentos",
          "[OptionsParser]") {
  using nicole::Error;
  using nicole::ERROR_TYPE;
  using nicole::Options;
  using nicole::OptionsParser;

  SECTION("Sin argumentos (tamaño 0)") {
    auto result = OptionsParser::parse(std::vector<std::string_view>{});
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().type() == ERROR_TYPE::MISSING_ARGUMENTS);
  }

  SECTION("Un argumento (tamaño 1)") {
    SECTION("Solo archivo de entrada") {
      auto result =
          OptionsParser::parse(std::vector<std::string_view>{"archivo.cpp"});
      REQUIRE(result.has_value());
      REQUIRE(result->entryFilePath() == "archivo.cpp");
      REQUIRE(result->binaryName() == "a.out");
      REQUIRE_FALSE(result->help());
      REQUIRE_FALSE(result->optimize());
    }

    SECTION("Opción -h") {
      auto result = OptionsParser::parse({"-h"});
      REQUIRE(result.has_value());
      REQUIRE(result->help());
      REQUIRE(result->binaryName() == "a.out");
      REQUIRE(result->entryFilePath().empty());
      REQUIRE_FALSE(result->optimize());
    }

    SECTION("Opción --help") {
      auto result = OptionsParser::parse({"--help"});
      REQUIRE(result.has_value());
      REQUIRE(result->help());
      REQUIRE(result->binaryName() == "a.out");
      REQUIRE(result->entryFilePath().empty());
      REQUIRE_FALSE(result->optimize());
    }

    SECTION("Opción -o sin archivo de entrada") {
      auto result = OptionsParser::parse(std::vector<std::string_view>{"-o"});
      REQUIRE_FALSE(result.has_value());
      REQUIRE(result.error().type() == ERROR_TYPE::MISSING_ENTRY_FILE);
    }

    SECTION("Opción -n sin nombre de binario") {
      auto result = OptionsParser::parse(std::vector<std::string_view>{"-n"});
      REQUIRE_FALSE(result.has_value());
      REQUIRE(result.error().type() == ERROR_TYPE::MISSING_BINARY_NAME);
    }

    SECTION("Opción desconocida") {
      auto result = OptionsParser::parse(std::vector<std::string_view>{"-x"});
      REQUIRE(result.has_value());
      REQUIRE(result->entryFilePath() == "-x");
      REQUIRE(result->binaryName() == "a.out");
      REQUIRE_FALSE(result->help());
      REQUIRE_FALSE(result->optimize());
    }
  }

  SECTION("Dos argumentos (tamaño 2)") {
    SECTION("Opción -h y archivo de entrada") {
      auto result = OptionsParser::parse(
          std::vector<std::string_view>{"-h", "archivo.cpp"});
      REQUIRE(result.has_value());
      REQUIRE(result->help());
      REQUIRE(result->entryFilePath() == "archivo.cpp");
      REQUIRE(result->binaryName() == "a.out");
      REQUIRE_FALSE(result->optimize());
    }

    SECTION("Archivo de entrada y opción -h") {
      auto result = OptionsParser::parse(
          std::vector<std::string_view>{"archivo.cpp", "-h"});
      REQUIRE(result.has_value());
      REQUIRE(result->help());
      REQUIRE(result->entryFilePath() == "archivo.cpp");
      REQUIRE(result->binaryName() == "a.out");
      REQUIRE_FALSE(result->optimize());
    }

    SECTION("Archivo de entrada y opción -o") {
      auto result = OptionsParser::parse(
          std::vector<std::string_view>{"archivo.cpp", "-o"});
      REQUIRE(result.has_value());
      REQUIRE(result->entryFilePath() == "archivo.cpp");
      REQUIRE(result->binaryName() == "a.out");
      REQUIRE_FALSE(result->help());
      REQUIRE(result->optimize());
    }

    SECTION("Opción -o y archivo de entrada") {
      auto result = OptionsParser::parse(
          std::vector<std::string_view>{"-o", "archivo.cpp"});
      REQUIRE(result.has_value());
      REQUIRE(result->entryFilePath() == "archivo.cpp");
      REQUIRE(result->binaryName() == "a.out");
      REQUIRE_FALSE(result->help());
      REQUIRE(result->optimize());
    }

    SECTION("Opción -n seguida de nombre de binario") {
      auto result = OptionsParser::parse(
          std::vector<std::string_view>{"-n", "miBinario"});
      REQUIRE_FALSE(result.has_value());
      REQUIRE(result.error().type() == ERROR_TYPE::MISSING_ENTRY_FILE);
    }

    SECTION("Archivo de entrada y opción -n sin nombre de binario") {
      auto result = OptionsParser::parse(
          std::vector<std::string_view>{"archivo.cpp", "-n"});
      REQUIRE_FALSE(result.has_value());
      REQUIRE(result.error().type() == ERROR_TYPE::MISSING_BINARY_NAME);
    }
  }

  SECTION("Tres argumentos (tamaño 3)") {
    SECTION("Opción -h, opción -o y archivo de entrada") {
      auto result = OptionsParser::parse(
          std::vector<std::string_view>{"-h", "-o", "archivo.cpp"});
      REQUIRE(result.has_value());
      REQUIRE(result->help());
      REQUIRE(result->entryFilePath() == "archivo.cpp");
      REQUIRE(result->binaryName() == "a.out");
      REQUIRE(result->optimize());
    }

    SECTION("Archivo de entrada, opción -h y opción -o") {
      auto result = OptionsParser::parse(
          std::vector<std::string_view>{"archivo.cpp", "-h", "-o"});
      REQUIRE(result.has_value());
      REQUIRE(result->help());
      REQUIRE(result->entryFilePath() == "archivo.cpp");
      REQUIRE(result->binaryName() == "a.out");
      REQUIRE(result->optimize());
    }

    SECTION("Opción -o, opción -h y archivo de entrada") {
      auto result = OptionsParser::parse(
          std::vector<std::string_view>{"-o", "-h", "archivo.cpp"});
      REQUIRE(result.has_value());
      REQUIRE(result->help());
      REQUIRE(result->entryFilePath() == "archivo.cpp");
      REQUIRE(result->binaryName() == "a.out");
      REQUIRE(result->optimize());
    }

    SECTION("Archivo de entrada, opción -o y opción -n con nombre de binario") {
      auto result = OptionsParser::parse(std::vector<std::string_view>{
          "archivo.cpp", "-o", "-n", "miBinario"});
      REQUIRE(result.has_value());
      REQUIRE(result->entryFilePath() == "archivo.cpp");
      REQUIRE(result->binaryName() == "miBinario");
      REQUIRE_FALSE(result->help());
      REQUIRE(result->optimize());
    }

    SECTION("Opción -o, archivo de entrada y opción -n con nombre de binario") {
      auto result = OptionsParser::parse(std::vector<std::string_view>{
          "-o", "archivo.cpp", "-n", "miBinario"});
      REQUIRE(result.has_value());
      REQUIRE(result->entryFilePath() == "archivo.cpp");
      REQUIRE(result->binaryName() == "miBinario");
      REQUIRE_FALSE(result->help());
      REQUIRE(result->optimize());
    }

    SECTION("Opción -n con nombre de binario, opción -o y archivo de entrada") {
      auto result = OptionsParser::parse(std::vector<std::string_view>{
          "-n", "miBinario", "-o", "archivo.cpp"});
      REQUIRE(result.has_value());
      REQUIRE(result->entryFilePath() == "archivo.cpp");
      REQUIRE(result->binaryName() == "miBinario");
      REQUIRE_FALSE(result->help());
      REQUIRE(result->optimize());
    }

    SECTION("Opción -n sin nombre de binario y archivo de entrada") {
      auto result = OptionsParser::parse(
          std::vector<std::string_view>{"-n", "archivo.cpp", "-o"});
      REQUIRE_FALSE(result.has_value());
      REQUIRE(result.error().type() == ERROR_TYPE::MISSING_ENTRY_FILE);
    }
  }

  SECTION("Cuatro argumentos (tamaño 4)") {
    SECTION("Opción -h, opción -o, opción -n con nombre de binario y archivo "
            "de entrada") {
      auto result = OptionsParser::parse(std::vector<std::string_view>{
          "-h", "-o", "-n", "miBinario", "archivo.cpp"});
      REQUIRE(result.has_value());
      REQUIRE(result->help());
      REQUIRE(result->entryFilePath() == "archivo.cpp");
      REQUIRE(result->binaryName() == "miBinario");
      REQUIRE(result->optimize());
    }

    SECTION("Archivo de entrada, opción -h, opción -o y opción -n con nombre "
            "de binario") {
      auto result = OptionsParser::parse(std::vector<std::string_view>{
          "archivo.cpp", "-h", "-o", "-n", "miBinario"});
      REQUIRE(result.has_value());
      REQUIRE(result->help());
      REQUIRE(result->entryFilePath() == "archivo.cpp");
      REQUIRE(result->binaryName() == "miBinario");
      REQUIRE(result->optimize());
    }

    SECTION("Opción -n con nombre de binario, opción -h, opción -o y archivo "
            "de entrada") {
      auto result = OptionsParser::parse(std::vector<std::string_view>{
          "-n", "miBinario", "-h", "-o", "archivo.cpp"});
      REQUIRE(result.has_value());
      REQUIRE(result->help());
      REQUIRE(result->entryFilePath() == "archivo.cpp");
      REQUIRE(result->binaryName() == "miBinario");
      REQUIRE(result->optimize());
    }
  }

  SECTION("Opción -o, opción -n con nombre de binario y archivo de entrada") {
    auto result = OptionsParser::parse(
        std::vector<std::string_view>{"-o", "-n", "miBinario", "archivo.cpp"});
    REQUIRE(result.has_value());
    REQUIRE(result->entryFilePath() == "archivo.cpp");
    REQUIRE(result->binaryName() == "miBinario");
    REQUIRE_FALSE(result->help());
    REQUIRE(result->optimize());
  }

  SECTION("Archivo de entrada, opción -o y opción -n con nombre de binario") {
    auto result = OptionsParser::parse(
        std::vector<std::string_view>{"archivo.cpp", "-o", "-n", "miBinario"});
    REQUIRE(result.has_value());
    REQUIRE(result->entryFilePath() == "archivo.cpp");
    REQUIRE(result->binaryName() == "miBinario");
    REQUIRE_FALSE(result->help());
    REQUIRE(result->optimize());
  }

  SECTION("Opción -n con nombre de binario, opción -o y archivo de entrada") {
    auto result = OptionsParser::parse(
        std::vector<std::string_view>{"-n", "miBinario", "-o", "archivo.cpp"});
    REQUIRE(result.has_value());
    REQUIRE(result->entryFilePath() == "archivo.cpp");
    REQUIRE(result->binaryName() == "miBinario");
    REQUIRE_FALSE(result->help());
    REQUIRE(result->optimize());
  }

  SECTION("Opción -n sin nombre de binario, opción -o y archivo de entrada") {
    auto result = OptionsParser::parse(
        std::vector<std::string_view>{"-n", "-o", "archivo.cpp", "miBinario"});
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().type() == ERROR_TYPE::MISSING_BINARY_NAME);
  }
}
*/
