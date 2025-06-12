#include "../inc/compiler/nicole.h"
#include "../inc/lexicalAnalysis/nicoleSintax.h"
#include "../inc/options/optionsParser.h"
#include <llvm/Support/InitLLVM.h>
#include <memory>

// valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all
// --track-origins=yes --error-exitcode=1 ./build/bin/binaries
// examples/super.nc

auto main(int argc, char *argv[]) noexcept -> int {
  llvm::InitLLVM initLLVM(argc, argv);
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
    std::cerr << compiled.error() << "\n";
    return 2;
  }
  return EXIT_SUCCESS;
}