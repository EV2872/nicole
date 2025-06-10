#ifndef NICOLÉ_OPTIMIZER_H
#define NICOLÉ_OPTIMIZER_H

#include "../errors.h"
#include <expected>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/IR/Module.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Target/TargetMachine.h>

namespace nicole {

class Optimizer final {
private:
  llvm::LLVMTargetMachine *targetMachine_;
  llvm::OptimizationLevel optLevel_;

public:
  Optimizer(llvm::LLVMTargetMachine *targetMachine,
            llvm::OptimizationLevel optLevel) noexcept
    : targetMachine_{targetMachine}, optLevel_{optLevel} {}

  Optimizer(const Optimizer &) = delete;
  Optimizer &operator=(const Optimizer &) = delete;

  [[nodiscard]] std::expected<llvm::orc::ThreadSafeModule, Error>
  optimize(llvm::orc::ThreadSafeModule TSM) noexcept;
};

} // namespace nicole

#endif // NICOLÉ_OPTIMIZER_H
