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
  std::shared_ptr<llvm::TargetMachine> targetMachine_;
  llvm::OptimizationLevel optLevel_;

public:
  Optimizer(std::shared_ptr<llvm::TargetMachine> targetMachine,
            llvm::OptimizationLevel optLevel) noexcept
      : targetMachine_{targetMachine}, optLevel_{optLevel} {}

  Optimizer(const Optimizer &) = delete;

  Optimizer &operator=(const Optimizer &) = delete;

  [[nodiscard]] auto optimize(llvm::orc::ThreadSafeModule TSM) noexcept
      -> std::expected<llvm::orc::ThreadSafeModule, Error>;
};

} // namespace nicole

#endif // NICOLÉ_OPTIMIZER_H
