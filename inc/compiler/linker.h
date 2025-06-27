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
  std::shared_ptr<llvm::TargetMachine> targetMachine_;

  [[nodiscard]] auto
  emitObjectFile(llvm::orc::ThreadSafeModule safeModule) const noexcept
      -> std::expected<std::monostate, Error>;

  [[nodiscard]] auto removeObjectFile() const noexcept
      -> std::expected<std::monostate, Error>;

public:
  explicit Linker(const Options &options,
                  std::shared_ptr<llvm::TargetMachine> targetMachine) noexcept
      : options_{options}, targetMachine_{targetMachine} {}

  [[nodiscard]] auto link(llvm::orc::ThreadSafeModule safeModule) const noexcept
      -> std::expected<std::monostate, Error>;
};

} // namespace nicole

#endif