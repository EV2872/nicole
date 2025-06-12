#ifndef VOID_TYPE_H
#define VOID_TYPE_H

#include "../type.h"
#include <string>

namespace nicole {

class VoidType final : public Type {
public:
  [[nodiscard]] auto toString() const noexcept->std::string override {
    return "void";
  }

  [[nodiscard]] auto 
  llvmVersion(llvm::LLVMContext &context) const noexcept->std::expected<llvm::Type*, Error> override {
    // Representamos void en LLVM como el tipo VoidTy
    return llvm::Type::getVoidTy(context);
  }
};

} // namespace nicole

#endif