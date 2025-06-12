#include "../../inc/compiler/linker.h"
#include <llvm-c/TargetMachine.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Program.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

namespace nicole {

auto Linker::emitObjectFile(llvm::orc::ThreadSafeModule safeModule)
    const noexcept -> std::expected<std::monostate, Error> {
  // Ajustar el módulo
  auto module{safeModule.getModuleUnlocked()};
  module->setTargetTriple(targetMachine_->getTargetCPU());
  module->setDataLayout(targetMachine_->createDataLayout());

  // Abrir flujo de salida para el objeto
  std::error_code EC;
  llvm::raw_fd_ostream Dest(options_.binaryName() + ".o", EC,
                            llvm::sys::fs::OF_None);
  if (EC) {
    return createError(ERROR_TYPE::ENTRY_FILE_NOT_FOUND,
                       "Error al abrir " + options_.binaryName() +
                           EC.message());
  }

  // Añadir pases para emitir el objeto y ejecutarlos
  llvm::legacy::PassManager PM;
  if (targetMachine_->addPassesToEmitFile(PM, Dest,
                                          /*DwoDest=*/nullptr,
                                          llvm::CodeGenFileType::ObjectFile)) {
    return createError(ERROR_TYPE::ENTRY_FILE_NOT_FOUND,
                       "Este TargetMachine no puede emitir objeto");
  }
  PM.run(*module);
  Dest.flush();

  return {};
}

auto Linker::removeObjectFile() const noexcept
    -> std::expected<std::monostate, Error> {
  const std::error_code removedObejctFile{
      llvm::sys::fs::remove(options_.binaryName() + ".o")};
  if (removedObejctFile) {
    return createError(ERROR_TYPE::MISSING_BINARY_NAME,
                       removedObejctFile.message());
  }
  return {};
}

auto Linker::link(llvm::orc::ThreadSafeModule safeModule) const noexcept
    -> std::expected<std::monostate, Error> {
  const auto emited{emitObjectFile(std::move(safeModule))};
  if (!emited) {
    return createError(emited.error());
  }
  std::string commandStr{"clang++ "};
  if (options_.useLLD()) {
    if (!llvm::sys::findProgramByName("lld")) {
      return createError(ERROR_TYPE::EAT, "no porgram called lld found");
    }
    commandStr += "-fuse-ld=lld ";
  }
  system(std::string{commandStr + options_.binaryName() + ".o -o " +
                     options_.binaryName()}
             .c_str());
  const auto removedObjects{removeObjectFile()};
  if (!removedObjects) {
    return createError(removedObjects.error());
  }

  return {};
}

} // namespace nicole