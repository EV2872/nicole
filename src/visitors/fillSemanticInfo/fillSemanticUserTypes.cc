#include "../../../inc/parsingAnalysis/ast/userTypes/ast_attrAccess.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_constructorCall.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_methodCall.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_struct.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_this.h"
#include "../../../inc/visitors/fillSemanticInfo/fillSemanticInfo.h"
#include <cstddef>
#include <memory>
#include <variant>
#include <vector>

namespace nicole {

auto FillSemanticInfo::visit(const AST_STRUCT *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_STRUCT");
  }

  analyzingInsideClass = true;

  currentStructGenericList_ = node->generics();

  if (hasDuplicatedGenerics(currentStructGenericList_)) {
    return createError(ERROR_TYPE::TYPE, "has duplicated generics");
  }

  std::shared_ptr<UserType> father{nullptr};
  if (node->fatherType()) {
    const auto instanceType =
        std::dynamic_pointer_cast<GenericInstanceType>(node->fatherType());
    if (instanceType) {
      if (typeTable_->isGenericType(instanceType, currentStructGenericList_)) {
        return createError(ERROR_TYPE::TYPE,
                           instanceType->toString() +
                               " is a generic so you cannot extend from it");
      }
      auto fatherExpected = typeTable_->getType(instanceType->name());
      if (!fatherExpected) {
        return createError(fatherExpected.error());
      }
      father = std::dynamic_pointer_cast<UserType>(fatherExpected.value());
      if (!father) {
        return createError(ERROR_TYPE::TYPE,
                           "The father type is not a UserType");
      }
    } else {
      const auto userType =
          std::dynamic_pointer_cast<UserType>(node->fatherType());
      if (!userType) {
        return createError(
            ERROR_TYPE::TYPE,
            "father type can only be a user type or generic instance");
      }
      if (typeTable_->isGenericType(userType, currentStructGenericList_)) {
        return createError(ERROR_TYPE::TYPE,
                           userType->toString() +
                               " is a generic so you cannot extend from it");
      }
      auto fatherExpected = typeTable_->getType(userType->name());
      if (!fatherExpected) {
        return createError(fatherExpected.error());
      }
      father = std::dynamic_pointer_cast<UserType>(fatherExpected.value());
      if (!father) {
        return createError(ERROR_TYPE::TYPE,
                           "The father type is not a UserType");
      }
    }
  }
  node->setFatherType(father);
  currentUserType_ =
      std::make_shared<UserType>(node->id(), father, node->generics());

  const auto insertType{typeTable_->insert(currentUserType_)};
  if (!insertType) {
    return createError(insertType.error());
  }


  size_t pos{0};
  std::vector<std::pair<std::string, std::shared_ptr<nicole::Type>>> attributes;
  for (const auto &attr : node->attributes()) {
    auto attrType = attr.second;
    if (auto maskedEnum = typeTable_->isCompundEnumType(attrType))
      attrType = *maskedEnum;

    if (auto maskedUserType = typeTable_->isCompundUserType(attrType))
      attrType = *maskedUserType;

    if (auto maskedGeneric =
            typeTable_->isCompundGenericType(attrType, currentGenericList_))
      attrType = *maskedGeneric;
    attributes.push_back({attr.first, attrType});
    const auto insertResult =
        currentUserType_->insertAttr(Attribute{attr.first, attrType, pos});
    if (!insertResult)
      return createError(insertResult.error());
    ++pos;
  }
  node->setAttributes(Attributes{attributes});

  for (const auto &method : node->methods()) {
    const auto result{method->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }

  const auto constructor{node->constructor()->accept(*this)};
  if (!constructor) {
    return createError(constructor.error());
  }

  const auto destructor{node->destructor()->accept(*this)};
  if (!destructor) {
    return createError(destructor.error());
  }


  analyzingInsideClass = false;
  currentStructGenericList_.clear();
  currentGenericList_.clear();
  for (const auto &methodDecl : node->methods()) {
    if (methodDecl->isVirtual() and !node->destructor()->isVirtual()) {
      return createError(
          ERROR_TYPE::METHOD,
          "a method cannot be virtual if the destructor is not virtual too");
    }
  }
  return {};
}

auto FillSemanticInfo::visit(const AST_ATTR_ACCESS *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ATTR_ACCESS");
  }
  return {};
}

auto FillSemanticInfo::visit(const AST_METHOD_CALL *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_METHOD_CALL");
  }
  for (std::size_t i = 0; i < node->replaceOfGenerics().size(); ++i) {
    auto replacement = node->replaceOfGenerics()[i];
    if (!typeTable_->isPossibleType(replacement) &&
        !typeTable_->isGenericType(replacement, currentGenericList_)) {
      return createError(ERROR_TYPE::TYPE,
                         replacement->toString() +
                             " is not a possible type or generic");
    }

    if (auto maskedEnum = typeTable_->isCompundEnumType(replacement)) {
      replacement = *maskedEnum;
      auto setRes = node->setGenericReplacement(i, replacement);
      if (!setRes)
        return createError(setRes.error());
    }

    if (auto maskedUserType = typeTable_->isCompundUserType(replacement)) {
      replacement = *maskedUserType;
      auto setRes = node->setGenericReplacement(i, replacement);
      if (!setRes)
        return createError(setRes.error());
    }

    if (auto maskedGeneric = typeTable_->isCompundGenericType(
            replacement, currentGenericList_)) {
      replacement = *maskedGeneric;
      auto setRes = node->setGenericReplacement(i, replacement);
      if (!setRes)
        return createError(setRes.error());
    }
  }

  for (const auto &expr : node->parameters()) {
    const auto result = expr->accept(*this);
    if (!result)
      return createError(result.error());
  }
  return {};
}

auto FillSemanticInfo::visit(const AST_METHOD_DECL *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_METHOD_DECL");

  currentGenericList_ = node->generics();
  auto mergedGenerics =
      mergeGenericLists(currentStructGenericList_, currentGenericList_);
  if (!mergedGenerics)
    return createError(mergedGenerics.error());
  currentGenericList_ = *mergedGenerics;
  node->setGenerics(currentGenericList_);

  pushScope();
  node->body()->setScope(currentScope_);

  std::vector<std::pair<std::string, std::shared_ptr<Type>>> updatedParams;
  for (const auto &param : node->parameters()) {
    if (currentUserType_->hasAttribute(param.first))
      return createError(ERROR_TYPE::ATTR, "Variable " + param.first +
                                               " is shadowing an attribute");

    if (!typeTable_->isPossibleType(param.second) &&
        !typeTable_->isGenericType(param.second, currentGenericList_))
      return createError(ERROR_TYPE::TYPE,
                         param.second->toString() +
                             " is not a possible type or generic");

    auto newType = param.second;
    if (auto maskedEnum = typeTable_->isCompundEnumType(newType))
      newType = *maskedEnum;
    if (auto maskedUserType = typeTable_->isCompundUserType(newType))
      newType = *maskedUserType;
    if (auto maskedGeneric =
            typeTable_->isCompundGenericType(newType, currentGenericList_))
      newType = *maskedGeneric;

    updatedParams.push_back({param.first, newType});

    if (auto insertVar = currentScope_->insert(
            std::make_shared<Variable>(param.first, newType, nullptr));
        !insertVar)
      return createError(insertVar.error());
  }
  Parameters params{updatedParams};
  node->setParameters(params);

  if (!typeTable_->isPossibleType(node->returnType()) &&
      !typeTable_->isGenericType(node->returnType(), currentGenericList_))
    return createError(ERROR_TYPE::TYPE,
                       node->returnType()->toString() +
                           " is not a possible type or generic");

  auto retType = node->returnType();
  if (auto maskedReturn = typeTable_->isCompundEnumType(retType))
    retType = *maskedReturn;
  if (auto maskedUserTypeReturn = typeTable_->isCompundUserType(retType))
    retType = *maskedUserTypeReturn;
  if (auto maskedGenericReturn =
          typeTable_->isCompundGenericType(retType, currentGenericList_))
    retType = *maskedGenericReturn;
  node->setReturnType(retType);

  Method newMethod{node->id(), currentGenericList_, params,
                   retType,    node->body(),        node->isVirtual()};

  const auto combinedMethods = currentUserType_->getMethods(node->id());
  if (combinedMethods) {
    for (const auto &existingMethod : *combinedMethods) {
      if (areAmbiguousMethods(existingMethod, newMethod)) {
        if (!existingMethod.isInherited() ||
            (existingMethod.isInherited() && !existingMethod.isVirtual()))
          return createError(ERROR_TYPE::METHOD,
                             "Ambiguous method declaration for: " + node->id());
      }
    }
  }

  currentUserType_->insertMethod(newMethod);
  // std::cout << "~~~~~~~~~~~~~~~~~~~~~~"<<
  // currentUserType_->getMethods("toString")->size() << "\n";

  const auto bodyResult = node->body()->accept(*this);
  if (!bodyResult)
    return createError(bodyResult.error());

  popScope();
  currentGenericList_.clear();
  return {};
}

auto FillSemanticInfo::visit(const AST_CONSTRUCTOR_DECL *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_CONSTRUCTOR_DECL");

  currentGenericList_ = node->generics();
  auto mergedGenerics =
      mergeGenericLists(currentStructGenericList_, currentGenericList_);
  if (!mergedGenerics)
    return createError(mergedGenerics.error());
  currentGenericList_ = *mergedGenerics;
  node->setGenerics(currentGenericList_);

  pushScope();
  node->body()->setScope(currentScope_);

  std::vector<std::pair<std::string, std::shared_ptr<nicole::Type>>>
      updatedParams;
  for (const auto &param : node->parameters()) {
    if (currentUserType_->hasAttribute(param.first))
      return createError(ERROR_TYPE::ATTR, "Variable " + param.first +
                                               " is shadowing an attribute");

    if (!typeTable_->isPossibleType(param.second) &&
        !typeTable_->isGenericType(param.second, currentGenericList_))
      return createError(ERROR_TYPE::TYPE,
                         param.second->toString() +
                             " is not a possible type or generic");

    auto newType = param.second;
    if (auto masked = typeTable_->isCompundEnumType(newType))
      newType = *masked;
    if (auto maskedUserType = typeTable_->isCompundUserType(newType))
      newType = *maskedUserType;
    if (auto maskedGeneric =
            typeTable_->isCompundGenericType(newType, currentGenericList_))
      newType = *maskedGeneric;

    updatedParams.push_back({param.first, newType});
    if (auto insertResult = currentScope_->insert(
            std::make_shared<Variable>(param.first, newType, nullptr));
        !insertResult)
      return createError(insertResult.error());
  }
  Parameters params{updatedParams};
  node->setParameters(params);

  currentUserType_->setConstructor(std::make_shared<Constructor>(
      node->id(), node->generics(), params, node->returnType(), node->body()));

  if (node->super()) {
    if (auto superResult = node->super()->accept(*this); !superResult)
      return createError(superResult.error());
  }

  if (!typeTable_->isPossibleType(node->returnType()) &&
      !typeTable_->isGenericType(node->returnType(), currentGenericList_))
    return createError(ERROR_TYPE::TYPE,
                       node->returnType()->toString() +
                           " is not a possible type or generic");

  auto retType = node->returnType();
  if (auto masked = typeTable_->isCompundEnumType(retType))
    retType = *masked;
  if (auto maskedUserType = typeTable_->isCompundUserType(retType))
    retType = *maskedUserType;
  if (auto maskedGeneric =
          typeTable_->isCompundGenericType(retType, currentGenericList_))
    retType = *maskedGeneric;
  node->setReturnType(retType);

  if (auto bodyResult = node->body()->accept(*this); !bodyResult)
    return createError(bodyResult.error());

  popScope();
  currentGenericList_.clear();
  return {};
}

auto FillSemanticInfo::visit(const AST_SUPER *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_SUPER");
  }

  const auto &reps = node->replacements();
  for (std::size_t i = 0; i < reps.size(); ++i) {
    auto replacement = reps[i];

    if (!typeTable_->isPossibleType(replacement) &&
        !typeTable_->isGenericType(replacement, currentGenericList_)) {
      return createError(ERROR_TYPE::TYPE,
                         replacement->toString() +
                             " is not a possible type or generic");
    }

    if (auto maskedEnum = typeTable_->isCompundEnumType(replacement)) {
      replacement = *maskedEnum;
      auto res = node->setGenericReplacement(i, replacement);
      if (!res) {
        return createError(res.error());
      }
    }

    if (auto maskedUserType = typeTable_->isCompundUserType(replacement)) {
      replacement = *maskedUserType;
      auto res = node->setGenericReplacement(i, replacement);
      if (!res) {
        return createError(res.error());
      }
    }

    if (auto maskedGeneric = typeTable_->isCompundGenericType(
            replacement, currentGenericList_)) {
      replacement = *maskedGeneric;
      auto res = node->setGenericReplacement(i, replacement);
      if (!res) {
        return createError(res.error());
      }
    }
  }

  for (const auto &arg : node->arguments()) {
    const auto result{arg->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }

  return {};
}

auto FillSemanticInfo::visit(const AST_DESTRUCTOR_DECL *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_DESTRUCTOR_DECL");
  }

  currentUserType_->setDestructor(
      std::make_shared<Destructor>(node->id(), node->body()));

  pushScope();
  node->body()->setScope(currentScope_);

  const auto body{node->body()->accept(*this)};
  if (!body) {
    return createError(body.error());
  }

  if (!typeTable_->isPossibleType(node->returnType()) &&
      !typeTable_->isGenericType(node->returnType(), currentGenericList_))
    return createError(ERROR_TYPE::TYPE,
                       node->returnType()->toString() +
                           " is not a possible type or generic");

  auto retType = node->returnType();
  if (auto masked = typeTable_->isCompundEnumType(retType))
    retType = *masked;
  if (auto maskedUserType = typeTable_->isCompundUserType(retType))
    retType = *maskedUserType;
  if (auto maskedGeneric =
          typeTable_->isCompundGenericType(retType, currentGenericList_))
    retType = *maskedGeneric;
  node->setReturnType(retType);

  popScope();

  return {};
}

auto FillSemanticInfo::visit(const AST_THIS *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_THIS");
  }
  if (analyzingInsideClass) {
    node->setUserType(currentUserType_);
  }
  return {};
}

auto FillSemanticInfo::visit(const AST_CONSTRUCTOR_CALL *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_CONSTRUCTOR_CALL");
  }
  if (typeTable_->isGenericType(
          std::make_shared<UserType>(node->id(), nullptr,
                                     std::vector<GenericParameter>{}),
          currentGenericList_)) {
    return {};
  }
  auto type{typeTable_->getType(node->id())};
  if (!type) {
    return createError(ERROR_TYPE::TYPE,
                       "no type with id: " + node->id() + " exists");
  }
  if (const auto enumType{std::dynamic_pointer_cast<EnumType>(*type)}) {
    if (node->replaceOfGenerics().size()) {
      return createError(
          ERROR_TYPE::TYPE,
          "a enum cannot use generics replacements in it's constructor");
    }
    return {};
  }

  const auto isUserType{std::dynamic_pointer_cast<UserType>(*type)};
  if (node->replaceOfGenerics().size() != isUserType->genericParams().size()) {
    return createError(ERROR_TYPE::TYPE, "invalid replacements of list size");
  }

  for (const auto &replacement : node->replaceOfGenerics()) {
    if (!typeTable_->isPossibleType(replacement) and
        !typeTable_->isGenericType(replacement, currentGenericList_)) {
      return createError(ERROR_TYPE::TYPE,
                         replacement->toString() +
                             " is not a posibble type or generic");
    }
  }

  for (const auto &expr : node->parameters()) {
    const auto resul{expr->accept(*this)};
    if (!resul) {
      return createError(resul.error());
    }
  }
  return {};
}

} // namespace nicole