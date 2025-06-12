#ifndef USER_TYPE_H
#define USER_TYPE_H

#include "attrTable.h"
#include "attribute.h"
#include "constructor.h"
#include "destructor.h"
#include "genericParameter.h"
#include "method.h"
#include "methodTable.h"
#include "virtualTable.h"
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

  [[nodiscard]] auto name() const noexcept -> const std::string & {
    return name_;
  }

  [[nodiscard]] auto baseType() const noexcept
      -> const std::shared_ptr<UserType> & {
    return baseType_;
  }

  [[nodiscard]] auto constructor() const noexcept
      -> const std::shared_ptr<Constructor> & {
    return constructor_;
  }

  [[nodiscard]] auto destructor() const noexcept
      -> const std::shared_ptr<Destructor> & {
    return destructor_;
  }

  [[nodiscard]] auto genericParams() const noexcept
      -> const std::vector<GenericParameter> & {
    return genericParams_;
  }

  [[nodiscard]] auto hasAttribute(const std::string &id) const noexcept -> bool;

  [[nodiscard]] auto hasMethod(const Method &id) const noexcept -> bool;

  [[nodiscard]] auto getAttribute(const std::string &id) const noexcept
      -> const std::expected<Attribute, Error>;

  [[nodiscard]] auto getMethods(const std::string &id) const noexcept
      -> const std::expected<std::vector<Method>, Error>;

  [[nodiscard]] auto attributes() const noexcept -> std::vector<Attribute> {
    std::vector<Attribute> result;
    result.reserve(attrTable_.size());
    for (auto &kv : attrTable_) {
      result.push_back(kv.second);
    }
    return result;
  }

  auto vtable() const noexcept -> const VirtualTable & { return vtable_; }

  auto
  setDestructor(const std::shared_ptr<Destructor> &destructor) const noexcept
      -> void {
    destructor_ = destructor;
  }

  auto
  setConstructor(const std::shared_ptr<Constructor> &constructor) const noexcept
      -> void {
    constructor_ = constructor;
  }

  [[nodiscard]] auto insertAttr(const Attribute &attr) const noexcept
      -> std::expected<std::monostate, Error>;

  auto insertMethod(const Method &method) const noexcept -> void;

  [[nodiscard]] auto setAttribute(const Attribute &attr) const noexcept
      -> std::expected<std::monostate, Error>;

  [[nodiscard]] auto
  isAboveInHearchy(const std::shared_ptr<UserType> &type) const noexcept
      -> bool;

  [[nodiscard]] auto toString() const noexcept -> std::string override;

  [[nodiscard]] auto llvmVersion(llvm::LLVMContext &context) const noexcept
      -> std::expected<llvm::Type *, Error> override;
};

} // namespace nicole

#endif