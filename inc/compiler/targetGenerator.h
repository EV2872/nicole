#ifndef TARGET_GENERATOR_H
#define TARGET_GENERATOR_H

#include "../options/options.h"
#include <llvm-c/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h> // legacy::PassManager
#include <llvm/MC/TargetRegistry.h>    // TargetRegistry::lookupTarget()
#include <llvm/Support/CodeGen.h>      // CodeGenFileType
#include <llvm/Support/FileSystem.h>   // sys::fs::OF_None
#include <llvm/Support/Program.h>
#include <llvm/Support/TargetSelect.h> // InitializeAllTargets()
#include <llvm/Support/raw_ostream.h>  // errs(), raw_fd_ostream
#include <llvm/Target/TargetMachine.h> // TargetMachine

namespace nicole {

class TargetGenerator {
public:
  [[nodiscard]] llvm::LLVMTargetMachine *generate(const Arch arch, const OS os) const noexcept;

private:
  [[nodiscard]] std::string getTripleForArchAndOS(Arch arch, OS os) const noexcept;
};

} // namespace nicole

#endif