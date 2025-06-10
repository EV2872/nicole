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
  static std::expected<std::unique_ptr<JIT>, Error> Create() noexcept;

  // Añade un ThreadSafeModule al JIT
  [[nodiscard]] std::expected<std::monostate, Error>
  addModule(llvm::orc::ThreadSafeModule tsm) noexcept;

  // Ejecuta la función 'fnName' y devuelve su entero de retorno
  [[nodiscard]] std::expected<int, Error>
  run(const std::string &fnName) noexcept;
};

} // namespace nicole

#endif