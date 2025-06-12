#include "../../inc/compiler/jitEngine.h"

namespace nicole {

auto JIT::Create() noexcept -> std::expected<std::unique_ptr<JIT>, Error> {
  auto ExpectedJIT = llvm::orc::LLJITBuilder().create();
  if (!ExpectedJIT) {
    return createError(ERROR_TYPE::FUNCTION,
                       "Error al crear LLJIT: " +
                           llvm::toString(ExpectedJIT.takeError()));
  }

  return std::unique_ptr<JIT>(new JIT(std::move(*ExpectedJIT)));
}

auto JIT::addModule(llvm::orc::ThreadSafeModule tsm) noexcept
    -> std::expected<std::monostate, Error> {
  if (auto Err = lljit_->addIRModule(std::move(tsm))) {
    return createError(ERROR_TYPE::FUNCTION,
                       "Error añadiendo módulo: " +
                           llvm::toString(std::move(Err)));
  }
  return std::monostate{};
}

auto JIT::run(const std::string &fnName) noexcept -> std::expected<int, Error> {
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