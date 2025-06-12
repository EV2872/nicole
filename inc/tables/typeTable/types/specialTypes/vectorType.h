#ifndef VECTOR_TYPE_H
#define VECTOR_TYPE_H

#include "../type.h"
#include <memory>
#include <string>

namespace nicole {

class VectorType final : public Type {
private:
  std::shared_ptr<Type> elementType_;

public:
  explicit VectorType(const std::shared_ptr<Type> &elementType)
      : elementType_{elementType} {}

  [[nodiscard]] auto elementType() const noexcept
      -> const std::shared_ptr<Type> & {
    return elementType_;
  }

  [[nodiscard]] auto toString() const noexcept -> std::string override {
    return "vector<" + elementType_->toString() + ">";
  }

  [[nodiscard]] auto llvmVersion(llvm::LLVMContext &context) const noexcept
      -> std::expected<llvm::Type *, Error> override {
    // Obtener el tipo LLVM correspondiente al tipo de elemento
    std::expected<llvm::Type *, Error> elemOrErr =
        elementType_->llvmVersion(context);
    if (!elemOrErr) {
      return std::unexpected(elemOrErr.error());
    }
    llvm::Type *elemTy = *elemOrErr;

    // Representar vector<T> como un struct { T* data; i64 size; }
    llvm::PointerType *dataPtrTy =
        llvm::PointerType::get(elemTy, /*AddressSpace=*/0);
    llvm::Type *sizeTy = llvm::Type::getInt64Ty(context);

    // Crear o recuperar un StructType único por nombre
    std::string structName = "vector<" + elementType_->toString() + ">";
    llvm::StructType *vecStruct =
        llvm::StructType::create(context, {dataPtrTy, sizeTy}, structName);

    return vecStruct;
  }
};

} // namespace nicole

#endif