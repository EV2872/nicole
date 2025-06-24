#include "../../inc/compiler/targetGenerator.h"

namespace nicole {

auto TargetGenerator::generate(const Arch arch, const OS os) const noexcept
    -> std::shared_ptr<llvm::TargetMachine> {
  // Inicializar backends (solo la primera vez)
  static bool initialized = false;
  if (!initialized) {
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
    initialized = true;
  }

  std::string tripleStr = getTripleForArchAndOS(arch, os);
  std::string error;
  const llvm::Target *target =
      llvm::TargetRegistry::lookupTarget(tripleStr, error);
  if (!target) {
    llvm::errs() << "Error encontrando target para " << tripleStr << ": "
                 << error << "\n";
    return nullptr;
  }

  llvm::TargetOptions opt;
  auto relocModel = std::optional<llvm::Reloc::Model>();
  llvm::TargetMachine *targetMachine{
      target->createTargetMachine(tripleStr, "generic", "", opt, relocModel)};
  return std::shared_ptr<llvm::TargetMachine>(
      static_cast<llvm::TargetMachine *>(targetMachine));
}

auto TargetGenerator::getTripleForArchAndOS(Arch arch, OS os) const noexcept
    -> std::string {
  if (arch == Arch::x86 && os == OS::Linux)
    return "i386-pc-linux-gnu";
  if (arch == Arch::x86 && os == OS::Windows)
    return "i386-pc-windows-msvc";
  if (arch == Arch::x86_64 && os == OS::Linux)
    return "x86_64-pc-linux-gnu";
  if (arch == Arch::x86_64 && os == OS::Windows)
    return "x86_64-pc-windows-msvc";
  if (arch == Arch::x86_64 && os == OS::MacOS)
    return "x86_64-apple-darwin";
  if (arch == Arch::arm && os == OS::Linux)
    return "armv7-none-linux-gnueabihf";
  if (arch == Arch::arm && os == OS::MacOS)
    return "arm-apple-darwin";
  if (arch == Arch::aarch64 && os == OS::Linux)
    return "aarch64-unknown-linux-gnu";
  if (arch == Arch::aarch64 && os == OS::MacOS)
    return "aarch64-apple-darwin";
  if (arch == Arch::aarch64 && os == OS::Windows)
    return "aarch64-pc-windows-msvc";

  return LLVMGetDefaultTargetTriple(); // Fallback
}

} // namespace nicole