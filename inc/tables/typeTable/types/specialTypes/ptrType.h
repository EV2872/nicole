#ifndef POINTER_TYPE_H
#define POINTER_TYPE_H

#include "../type.h"
#include <memory>
#include <string>

namespace nicole {

class PointerType final : public Type {
private:
  std::shared_ptr<Type> baseType_;

public:
  explicit PointerType(const std::shared_ptr<Type> &baseType)
      : baseType_{baseType} {}

  [[nodiscard]] auto baseType() const noexcept
      -> const std::shared_ptr<Type> & {
    return baseType_;
  }

  [[nodiscard]] auto toString() const noexcept -> std::string override {
    return baseType_->toString() + "*";
  }

  [[nodiscard]] auto llvmVersion(llvm::LLVMContext &context) const noexcept
      -> std::expected<llvm::Type *, Error> override {
    // Obtener el tipo LLVM del tipo base
    std::expected<llvm::Type *, Error> baseTyOrErr =
        baseType_->llvmVersion(context);
    if (!baseTyOrErr) {
      return std::unexpected(baseTyOrErr.error());
    }
    llvm::Type *baseTy = *baseTyOrErr;
    // Crear y retornar el puntero al tipo base
    return llvm::PointerType::get(baseTy, /*AddressSpace=*/0);
  }
};

} // namespace nicole

#endif