#include "../../../inc/tables/typeTable/types/userTypes/userType.h"
#include <iostream>
#include <memory>
#include <sstream>

namespace nicole {

auto UserType::hasAttribute(const std::string &id) const noexcept -> bool {
  if (baseType_) {
    if (const std::shared_ptr<UserType> userType =
            std::dynamic_pointer_cast<UserType>(baseType_)) {
      if (userType->hasAttribute(id)) {
        return true;
      }
    }
  }
  if (attrTable_.has(id)) {
    return true;
  }
  return false;
}

auto UserType::hasMethod(const Method &id) const noexcept -> bool {
  if (baseType_) {
    if (const std::shared_ptr<UserType> userType =
            std::dynamic_pointer_cast<UserType>(baseType_))
      return userType->hasMethod(id);
  }
  if (methodTable_.getMethods(id.id()).size()) {
    return true;
  }
  return false;
}

auto UserType::getAttribute(const std::string &id) const noexcept
    -> const std::expected<Attribute, Error> {
  if (attrTable_.has(id)) {
    return attrTable_.getAttribute(id);
  }
  if (baseType_) {
    if (const std::shared_ptr<UserType> userType =
            std::dynamic_pointer_cast<UserType>(baseType_)) {
      return userType->getAttribute(id);
    }
  }
  return createError(ERROR_TYPE::ATTR,
                     "the attribute: " + id + " does not exist in " + name_);
}

auto UserType::getMethods(const std::string &id) const noexcept
    -> const std::expected<std::vector<Method>, Error> {
  std::vector<Method> combinedMethods;
  if (methodTable_.getMethods(id).size()) {
    std::vector<Method> childRes = methodTable_.getMethods(id);
    combinedMethods.insert(combinedMethods.end(), childRes.begin(),
                           childRes.end());
  }
  if (baseType_) {
    if (const std::shared_ptr<UserType> userType =
            std::dynamic_pointer_cast<UserType>(baseType_)) {
      std::expected<std::vector<Method>, Error> parentRes =
          userType->getMethods(id);
      for (Method &method : parentRes.value()) {
        method.setInherit(true);
      }
      if (parentRes)
        combinedMethods.insert(combinedMethods.end(), parentRes.value().begin(),
                               parentRes.value().end());
    }
  }
  if (!combinedMethods.empty())
    return combinedMethods;

  return createError(ERROR_TYPE::METHOD,
                     "the method: " + id + " does not exist in " + name_);
}

auto UserType::insertAttr(const Attribute &attr) const noexcept
    -> std::expected<std::monostate, Error> {
  if (hasAttribute(attr.id())) {
    return createError(ERROR_TYPE::ATTR,
                       "the attribute: " + attr.id() + " already exists");
  }
  return attrTable_.insert(attr);
}

auto UserType::insertMethod(const Method &method) const noexcept -> void {
  methodTable_.insert(method);
}

auto UserType::setAttribute(const Attribute &attr) const noexcept
    -> std::expected<std::monostate, Error> {
  // Comprobar que el atributo existe en el UserType o en sus bases.
  if (!attrTable_.has(attr.id())) {
    return createError(ERROR_TYPE::ATTR, "Attribute: " + attr.id() +
                                             " does not exist in " + name_);
  }
  // Delegar la actualización al AttrTable.
  return attrTable_.setAttribute(attr);
}

auto UserType::isAboveInHearchy(
    const std::shared_ptr<UserType> &type) const noexcept -> bool {
  std::shared_ptr<UserType> aux{baseType_};
  while (aux) {
    if (aux->name_ == type->name_) {
      return true;
    }
    aux = aux->baseType_;
  }
  return false;
}

auto UserType::toString() const noexcept -> std::string {
  std::ostringstream oss;
  oss << name_;
  if (!genericParams_.empty()) {
    oss << "<";
    for (size_t i = 0; i < genericParams_.size(); ++i) {
      oss << genericParams_[i].name();
      if (i != genericParams_.size() - 1)
        oss << ", ";
    }
    oss << ">";
  }
  if (baseType_) {
    oss << " : " << baseType_->toString();
  }
  return oss.str();
}

auto UserType::llvmVersion(llvm::LLVMContext &context) const noexcept
    -> std::expected<llvm::Type *, Error> {
  llvm::StructType *st = llvm::StructType::getTypeByName(context, name_);
  if (!st) {
    st = llvm::StructType::create(context, name_);
  }

  // Si es opaco, definir cuerpo aplanando herencia
  if (st->isOpaque()) {
    llvm::SmallVector<llvm::Type *, 4> elems;

    // Campos heredados (aplanados)
    if (baseType_) {
      auto baseOrErr = baseType_->llvmVersion(context);
      if (!baseOrErr)
        return std::unexpected(baseOrErr.error());
      auto *baseSt = llvm::cast<llvm::StructType>(*baseOrErr);
      for (unsigned i = 0, n = baseSt->getNumElements(); i < n; ++i) {
        elems.push_back(baseSt->getElementType(i));
      }
    }

    // Campos propios
    for (auto &kv : attrTable_) {
      auto &attr = kv.second;
      auto tyOrErr = attr.type()->llvmVersion(context);
      if (!tyOrErr)
        return std::unexpected(tyOrErr.error());
      attr.setPosition(elems.size());
      elems.push_back(*tyOrErr);
    }

    st->setBody(elems, /*isPacked=*/false);
  }

  return st;
}

} // namespace nicole