#include "../inc/compiler/nicole.h"
#include "../inc/options/optionsParser.h"

int main(int argc, char *argv[]) {
  const auto options = nicole::OptionsParser::parse(argc, argv);
  if (!options) {
    // Informar al usuario del error y salir
    std::cerr << "Error: " << options.error() << "\n";
    return EXIT_FAILURE;
  }
  const std::shared_ptr<nicole::Sintax> sintax{
      std::make_shared<nicole::NicoleSintax>()};
  const nicole::Nicole compiler{sintax};
  const auto compiled{compiler.compile(*options)};
  if (!compiled) {
    std::cerr << compiled.error() << "\n" << std::flush;
    return 2;
  }
  return EXIT_SUCCESS;
}