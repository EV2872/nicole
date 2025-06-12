#ifndef NULL_TYPE_H
#define NULL_TYPE_H

#include "../type.h"
#include <string>

namespace nicole {

class NullType final : public Type {
public:
  [[nodiscard]] auto toString() const noexcept -> std::string override {
    return "nullptr";
  }

  [[nodiscard]] auto llvmVersion(llvm::LLVMContext &context) const noexcept
      -> std::expected<llvm::Type *, Error> override {
    // Representamos nullptr como puntero genérico (i8*)
    llvm::Type *i8Ty = llvm::Type::getInt8Ty(context);
    llvm::PointerType *ptrTy = llvm::PointerType::get(i8Ty, /*AddressSpace=*/0);
    return ptrTy;
  }
};

} // namespace nicole

#endif