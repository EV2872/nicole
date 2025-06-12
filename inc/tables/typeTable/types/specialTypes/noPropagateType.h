#ifndef NO_PROPAGATE_TYPE_H
#define NO_PROPAGATE_TYPE_H

#include "../type.h"
#include <string>

namespace nicole {

class NoPropagateType final : public Type {
public:
  [[nodiscard]] auto toString() const noexcept -> std::string override {
    return "NoPropagateType";
  }

  [[nodiscard]] auto llvmVersion(llvm::LLVMContext &) const noexcept
      -> std::expected<llvm::Type *, Error> override {
    return createError(ERROR_TYPE::TYPE, "invalid llvm conversion");
  }
};

} // namespace nicole

#endif