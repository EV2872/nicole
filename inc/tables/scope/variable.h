#ifndef VARIABLE_H
#define VARIABLE_H

#include "../symbol.h"
#include "../typeTable/types/type.h"
#include <expected>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <memory>
#include <string>

namespace nicole {

class Variable final : public Symbol {
private:
  mutable std::shared_ptr<Type> type_;
  mutable llvm::Value *value_;
  mutable llvm::AllocaInst *address_;

public:
  explicit Variable(const std::string &id, const std::shared_ptr<Type> &type,
                    llvm::Value *value) noexcept
      : Symbol{id}, type_{type}, value_{value} {}

  [[nodiscard]] auto type() const noexcept -> const std::shared_ptr<Type> & {
    return type_;
  }

  [[nodiscard]] auto value() const noexcept -> llvm::Value *& { return value_; }

  [[nodiscard]] auto address() const noexcept -> llvm::AllocaInst * {
    return address_;
  }

  auto setType(const std::shared_ptr<Type> &type) const noexcept -> void {
    type_ = type;
  }

  auto setValue(llvm::Value *v) const noexcept -> void { value_ = v; }

  auto setAddress(llvm::AllocaInst *addr) const noexcept -> void {
    address_ = addr;
  }
};

} // namespace nicole

#endif