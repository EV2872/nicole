#ifndef USER_TYPE_H
#define USER_TYPE_H

#include "attrTable.h"
#include "virtualTable.h"
#include "attribute.h"
#include "constructor.h"
#include "destructor.h"
#include "genericParameter.h"
#include "method.h"
#include "methodTable.h"
#include <expected>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace nicole {

class UserType : public Type {
private:
  std::string name_;
  std::shared_ptr<UserType> baseType_; // Solo se permite una base
  std::vector<GenericParameter> genericParams_;
  mutable AttrTable attrTable_;
  mutable MethodTable methodTable_;
  mutable std::shared_ptr<Constructor> constructor_;
  mutable std::shared_ptr<Destructor> destructor_;
  mutable VirtualTable vtable_;

public:
  explicit UserType(const std::string &name,
                    const std::shared_ptr<UserType> &baseType,
                    const std::vector<GenericParameter> &genericParams) noexcept
      : name_{name}, baseType_(baseType), genericParams_{genericParams} {}

  [[nodiscard]] const std::string &name() const noexcept { return name_; }

  [[nodiscard]] const std::shared_ptr<UserType> &baseType() const noexcept {
    return baseType_;
  }

  [[nodiscard]] const std::shared_ptr<Constructor> &
  constructor() const noexcept {
    return constructor_;
  }

  [[nodiscard]] const std::shared_ptr<Destructor> &destructor() const noexcept {
    return destructor_;
  }

  [[nodiscard]] const std::vector<GenericParameter> &
  genericParams() const noexcept {
    return genericParams_;
  }

  [[nodiscard]] bool hasAttribute(const std::string &id) const noexcept;

  [[nodiscard]] bool hasMethod(const Method &id) const noexcept;

  [[nodiscard]] const std::expected<Attribute, Error>
  getAttribute(const std::string &id) const noexcept;

  [[nodiscard]] const std::expected<std::vector<Method>, Error>
  getMethods(const std::string &id) const noexcept;

  [[nodiscard]] std::vector<Attribute> attributes() const noexcept {
    std::vector<Attribute> result;
    result.reserve(attrTable_.size());
    for (auto &kv : attrTable_) {
      result.push_back(kv.second);
    }
    return result;
  }

  const VirtualTable &vtable() const noexcept { return vtable_; }

  void
  setDestructor(const std::shared_ptr<Destructor> &destructor) const noexcept {
    destructor_ = destructor;
  }

  void setConstructor(
      const std::shared_ptr<Constructor> &constructor) const noexcept {
    constructor_ = constructor;
  }

  [[nodiscard]] std::expected<std::monostate, Error>
  insertAttr(const Attribute &attr) const noexcept;

  void insertMethod(const Method &method) const noexcept;

  [[nodiscard]] std::expected<std::monostate, Error>
  setAttribute(const Attribute &attr) const noexcept;

  [[nodiscard]] bool
  isAboveInHearchy(const std::shared_ptr<UserType> &type) const noexcept;

  [[nodiscard]] std::string toString() const noexcept override;

  [[nodiscard]] std::expected<llvm::Type *, Error>
  llvmVersion(llvm::LLVMContext &context) const noexcept override;
};

} // namespace nicole

#endif