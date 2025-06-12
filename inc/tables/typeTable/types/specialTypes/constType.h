#ifndef CONST_TYPE_H
#define CONST_TYPE_H

#include "../type.h"
#include <memory>
#include <string>

namespace nicole {

class ConstType final : public Type {
private:
  std::shared_ptr<Type> baseType_;

public:
  explicit ConstType(const std::shared_ptr<Type> &baseType) noexcept
      : baseType_{baseType} {}

  [[nodiscard]] auto baseType() const noexcept
      -> const std::shared_ptr<Type> & {
    return baseType_;
  }

  [[nodiscard]] auto toString() const noexcept -> std::string override {
    return "const " + baseType_->toString();
  }

  [[nodiscard]] auto llvmVersion(llvm::LLVMContext &context) const noexcept
      -> std::expected<llvm::Type *, Error> override {
    return baseType_->llvmVersion(context);
  }
};

} // namespace nicole

#endif