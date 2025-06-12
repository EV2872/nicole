#ifndef BREAK_TYPE_H
#define BREAK_TYPE_H

#include "../type.h"
#include <string>

namespace nicole {

class BreakType final : public Type {
public:
  [[nodiscard]] auto toString() const noexcept -> std::string override {
    return "breakType";
  }

  [[nodiscard]] auto llvmVersion(llvm::LLVMContext &) const noexcept
      -> std::expected<llvm::Type *, Error> override {
    return createError(ERROR_TYPE::TYPE, "invalid llvm conversion");
  }
};

} // namespace nicole

#endif