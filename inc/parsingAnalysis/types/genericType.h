#ifndef GENERIC_TYPE_H
#define GENERIC_TYPE_H

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Value.h"
#include <llvm/IR/Type.h>
#include <memory>
#include <string>

namespace nicole {

// Abstract class to represent primitives and user types
class GenericType {
protected:
  /* data */
  std::string name_{""};
  std::weak_ptr<GenericType> father_;

public:
  GenericType(const std::string &name,
              std::shared_ptr<GenericType> father = nullptr)
      : name_{name}, father_{father} {};

  virtual ~GenericType() = default;

  virtual llvm::Type *type(llvm::LLVMContext *context) const = 0;

  std::string name() const { return name_; }

  std::shared_ptr<GenericType> father() const { return father_.lock(); }

  bool isSameType(const GenericType& type) const { return name_ == type.name(); }

  bool isSameType(const GenericType* type) const { return name_ == type->name(); }
};

} // namespace nicole

#endif