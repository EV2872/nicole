#include "../../inc/compiler/jitEngine.h"

namespace nicole {

std::expected<std::unique_ptr<JIT>, Error> JIT::Create() noexcept {
  auto ExpectedJIT = llvm::orc::LLJITBuilder().create();
  if (!ExpectedJIT) {
    return createError(ERROR_TYPE::FUNCTION,
                       "Error al crear LLJIT: " +
                           llvm::toString(ExpectedJIT.takeError()));
  }

  return std::unique_ptr<JIT>(new JIT(std::move(*ExpectedJIT)));
}

std::expected<std::monostate, Error>
JIT::addModule(llvm::orc::ThreadSafeModule tsm) noexcept {
  if (auto Err = lljit_->addIRModule(std::move(tsm))) {
    return createError(ERROR_TYPE::FUNCTION,
                       "Error añadiendo módulo: " +
                           llvm::toString(std::move(Err)));
  }
  return std::monostate{};
}

std::expected<int, Error>
  JIT::run(const std::string &fnName) noexcept {
    auto SymOrErr = lljit_->lookup(fnName);
    if (!SymOrErr) {
      return createError(ERROR_TYPE::FUNCTION,
                         "Función '" + fnName + "' no encontrada: " +
                             llvm::toString(SymOrErr.takeError()));
    }
    using FnType = int (*)();
    auto Addr = SymOrErr->getValue();
    if (!Addr) {
      return createError(ERROR_TYPE::FUNCTION,
                         "Dirección inválida para '" + fnName + "'");
    }
    FnType Fn = reinterpret_cast<FnType>(Addr);
    return Fn();
  }

} // namespace nicole