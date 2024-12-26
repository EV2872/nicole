#include "../inc/options/options.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Pruebas de la clase Options", "[options]") {
  using nicole::Options;
  using std::filesystem::path;

  SECTION("Constructor y métodos de acceso") {
    const Options opt{true, true, "binario", "ruta/archivo.cpp"};
    REQUIRE(opt.help() == true);
    REQUIRE(opt.optimize() == true);
    REQUIRE(opt.binaryName() == "binario");
    REQUIRE(opt.entryFilePath() == path{"ruta/archivo.cpp"});
  }

  SECTION("Valores por defecto") {
    const Options opt{false, false, "", ""};
    REQUIRE(opt.help() == false);
    REQUIRE(opt.optimize() == false);
    REQUIRE(opt.binaryName().empty());
    REQUIRE(opt.entryFilePath().empty());
  }

  SECTION("Modificación de atributos") {
    Options opt{false, false, "binario", "ruta/archivo.cpp"};
    // Simulando cambios en los atributos
    opt = Options{true, true, "nuevo_binario", "nueva_ruta/nuevo_archivo.cpp"};
    REQUIRE(opt.help() == true);
    REQUIRE(opt.optimize() == true);
    REQUIRE(opt.binaryName() == "nuevo_binario");
    REQUIRE(opt.entryFilePath() == path{"nueva_ruta/nuevo_archivo.cpp"});
  }

  SECTION("Comparación de objetos Options") {
    const Options opt1{true, false, "binario", "ruta/archivo.cpp"};
    const Options opt2{true, false, "binario", "ruta/archivo.cpp"};
    const Options opt3{false, true, "otro_binario",
                       "otra_ruta/otro_archivo.cpp"};

    REQUIRE(opt1.help() == opt2.help());
    REQUIRE(opt1.optimize() == opt2.optimize());
    REQUIRE(opt1.binaryName() == opt2.binaryName());
    REQUIRE(opt1.entryFilePath() == opt2.entryFilePath());

    REQUIRE(opt1.help() != opt3.help());
    REQUIRE(opt1.optimize() != opt3.optimize());
    REQUIRE(opt1.binaryName() != opt3.binaryName());
    REQUIRE(opt1.entryFilePath() != opt3.entryFilePath());
  }
}