#ifndef JIT_H
#define JIT_H

#include "../errors.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include <expected>
#include <memory>
#include <string>

namespace nicole {

class JIT {
  std::unique_ptr<llvm::orc::LLJIT> lljit_;

  explicit JIT(std::unique_ptr<llvm::orc::LLJIT> jit) noexcept
      : lljit_(std::move(jit)) {}

public:
  [[nodiscard]] static auto Create() noexcept
      -> std::expected<std::unique_ptr<JIT>, Error>;

  [[nodiscard]] auto addModule(llvm::orc::ThreadSafeModule tsm) noexcept
      -> std::expected<std::monostate, Error>;

  [[nodiscard]] auto run(const std::string &fnName) noexcept
      -> std::expected<int, Error>;
};

} // namespace nicole

#endif