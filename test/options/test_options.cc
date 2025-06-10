#include "../inc/options/options.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Pruebas de la clase Options", "[options]") {
  using nicole::Options;
  using std::filesystem::path;

  SECTION("Constructor y métodos de acceso") {
    const Options opt{true,
                      true,
                      true,
                      true,
                      true,
                      true,
                      "binario",
                      "ruta/archivo.cpp",
                      nicole::Arch::x86,
                      nicole::OS::Linux};
    REQUIRE(opt.optimize() == true);
    REQUIRE(opt.printTree() == true);
    REQUIRE(opt.printIR() == true);
    REQUIRE(opt.validateTree() == true);
    REQUIRE(opt.binaryName() == "binario");
    REQUIRE(opt.entryFilePath() == path{"ruta/archivo.cpp"});
  }

  SECTION("Valores por defecto") {
    const Options opt{false,
                      false,
                      true,
                      true,
                      true,
                      true,
                      "",
                      "",
                      nicole::Arch::x86,
                      nicole::OS::Linux};
    REQUIRE(opt.optimize() == false);
    REQUIRE(opt.binaryName().empty());
    REQUIRE(opt.entryFilePath().empty());
  }

  SECTION("Modificación de atributos") {
    Options opt{false,
                false,
                true,
                true,
                true,
                true,
                "binario",
                "ruta/archivo.cpp",
                nicole::Arch::x86,
                nicole::OS::Linux};
    // Simulando cambios en los atributos
    opt = Options{true,
                  true,
                  true,
                  true,
                  true,
                  true,
                  "nuevo_binario",
                  "nueva_ruta/nuevo_archivo.cpp",
                  nicole::Arch::x86,
                  nicole::OS::Linux};
    REQUIRE(opt.optimize() == true);
    REQUIRE(opt.binaryName() == "nuevo_binario");
    REQUIRE(opt.entryFilePath() == path{"nueva_ruta/nuevo_archivo.cpp"});
  }

  SECTION("Comparación de objetos Options") {
    const Options opt1{true,
                       false,
                       true,
                       true,
                       true,
                       true,
                       "binario",
                       "ruta/archivo.cpp",
                       nicole::Arch::x86,
                       nicole::OS::Linux};
    const Options opt2{true,
                       false,
                       true,
                       true,
                       true,
                       true,
                       "binario",
                       "ruta/archivo.cpp",
                       nicole::Arch::x86,
                       nicole::OS::Linux};
    const Options opt3{false,
                       true,
                       true,
                       true,
                       true,
                       true,
                       "otro_binario",
                       "otra_ruta/otro_archivo.cpp",
                       nicole::Arch::x86,
                       nicole::OS::Linux};

    REQUIRE(opt1.optimize() == opt2.optimize());
    REQUIRE(opt1.printTree() == opt2.printTree());
    REQUIRE(opt1.printIR() == opt2.printIR());
    REQUIRE(opt1.binaryName() == opt2.binaryName());
    REQUIRE(opt1.entryFilePath() == opt2.entryFilePath());

    REQUIRE(opt1.optimize() != opt3.optimize());
    REQUIRE(opt1.printTree() == opt3.printTree());
    REQUIRE(opt1.printIR() == opt3.printIR());

    REQUIRE(opt1.binaryName() != opt3.binaryName());
    REQUIRE(opt1.entryFilePath() != opt3.entryFilePath());
  }
}