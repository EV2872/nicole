#ifndef BASE_TYPE_H
#define BASE_TYPE_H

#include "../../../errors.h"
#include <expected>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <string>

namespace nicole {

class Type {
public:
  virtual ~Type() = default;

  [[nodiscard]] virtual auto toString() const noexcept -> std::string = 0;

  [[nodiscard]] virtual auto
  llvmVersion(llvm::LLVMContext &context) const noexcept
      -> std::expected<llvm::Type *, Error> = 0;
};

} // namespace nicole

#endif