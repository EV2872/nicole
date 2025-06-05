#ifndef USER_TYPE_H
#define USER_TYPE_H

#include "attrTable.h"
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
  llvmVersion(llvm::LLVMContext &context) const noexcept override {
    // Crear o recuperar el struct opaco con el nombre de la clase
    llvm::StructType *st = llvm::StructType::getTypeByName(context, name_);
    if (!st) {
      st = llvm::StructType::create(context, name_);
    }

    if (st->isOpaque()) {
      llvm::SmallVector<llvm::Type *, 4> elems;
      // Herencia: si baseType_ existe, primero su struct
      if (baseType_) {
        std::expected<llvm::Type *, Error> baseOrErr =
            baseType_->llvmVersion(context);
        if (!baseOrErr)
          return std::unexpected(baseOrErr.error());
        elems.push_back(*baseOrErr);
      }
      // Atributos: recorrer attrTable_ y añadir cada campo
      for (std::pair<const std::string, Attribute> &attr : attrTable_) {
        std::expected<llvm::Type *, Error> tyOrErr =
            attr.second.type()->llvmVersion(context);
        if (!tyOrErr)
          return std::unexpected(tyOrErr.error());
        attr.second.setPosition(elems.size());
        elems.push_back(*tyOrErr);
      }
      st->setBody(elems, /*isPacked=*/false);
    }
    return st;
  }
};

} // namespace nicole

#endif