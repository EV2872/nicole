#ifndef LINKER_H
#define LINKER_H

#include "../errors.h"
#include "../options/options.h"
#include <expected>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/IR/Module.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Target/TargetMachine.h>
#include <variant>


namespace nicole {

enum OPT_LEVEL { DEFAULT, MEDIUM, AGGRESSIVE };

class Linker final {
private:
  Options options_;
  llvm::LLVMTargetMachine *targetMachine_;

  [[nodiscard]] std::expected<std::monostate, Error>
  emitObjectFile(llvm::orc::ThreadSafeModule safeModule) const noexcept;

  [[nodiscard]] std::expected<std::monostate, Error>
  removeObjectFile() const noexcept;

public:
  explicit Linker(const Options &options,
                  llvm::LLVMTargetMachine *targetMachine) noexcept
      : options_{options}, targetMachine_{targetMachine} {}

  [[nodiscard]] std::expected<std::monostate, Error>
  link(llvm::orc::ThreadSafeModule safeModule) const noexcept;
};

} // namespace nicole

#endif