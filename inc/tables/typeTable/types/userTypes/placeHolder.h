#ifndef PLACE_HOLDER_H
#define PLACE_HOLDER_H

#include "../type.h"
#include "genericParameter.h"
#include <string>

namespace nicole {

class PlaceHolder final : public Type {
private:
  GenericParameter genericParameter_;

public:
  explicit PlaceHolder(const GenericParameter &genericParameter) noexcept
      : genericParameter_(genericParameter) {}

  [[nodiscard]] auto toString() const noexcept -> std::string override {
    return "PlaceHolder: " + genericParameter_.name();
  }

  [[nodiscard]] auto getGenericParameter() const noexcept
      -> const GenericParameter & {
    return genericParameter_;
  }

  [[nodiscard]] auto llvmVersion(llvm::LLVMContext &) const noexcept
      -> std::expected<llvm::Type *, Error> override {
    // Unresolved placeholder: no LLVM type can be generated
    return createError(ERROR_TYPE::TYPE,
                       "Unresolved generic parameter '" +
                           genericParameter_.name() +
                           "' cannot be lowered to LLVM type.");
  }
};

} // namespace nicole

#endif
