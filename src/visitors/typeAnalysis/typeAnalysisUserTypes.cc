#include "../../../inc/parsingAnalysis/ast/userTypes/ast_attrAccess.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_constructorCall.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_methodCall.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_struct.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_this.h"
#include "../../../inc/visitors/typeAnalysis/typeAnalysis.h"
#include <cstddef>
#include <memory>
#include <vector>

namespace nicole {

/*
- comprobar tipo atributos no sea void ni puntero a est, comprobar si es generic
- comrpobar metodos
- comproabr constructor
- comprobar destructor
- retorna NoPropagate
*/
std::expected<std::shared_ptr<Type>, Error>
TypeAnalysis::visit(const AST_STRUCT *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_STRUCT");
  }
  if (!node->generics().empty()) {
    insideDeclWithGenerics = true;
    currentGenericList_ = node->generics();
  }

  analyzingInsideClass = true;

  if (node->constructor()) {
    auto consRes = node->constructor()->accept(*this);
    if (!consRes)
      return createError(consRes.error());
  }

  if (node->destructor()) {
    auto destrRes = node->destructor()->accept(*this);
    if (!destrRes)
      return createError(destrRes.error());
  }

  for (const auto &method : node->methods()) {
    auto methodRes = method->accept(*this);
    if (!methodRes)
      return createError(methodRes.error());
  }

  insideDeclWithGenerics = false;
  currentGenericList_.clear();
  analyzingInsideClass = false;
  node->setReturnedFromAnalysis(typeTable_->noPropagateType());
  return typeTable_->noPropagateType();
}

/*
- comprobar que sea genric
- se apoya en currentType para saber si el atributo existe y retorna el tipo
*/
std::expected<std::shared_ptr<Type>, Error>
TypeAnalysis::visit(const AST_ATTR_ACCESS *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ATTR_ACCESS");
  }
  if (!currentType_)
    return createError(ERROR_TYPE::TYPE,
                       "current type not set for attribute access");

  auto baseType = currentType_;

  if (insideDeclWithGenerics &&
      typeTable_->isGenericType(baseType, currentGenericList_)) {
    return baseType;
  }

  auto userType = std::dynamic_pointer_cast<UserType>(baseType);
  if (!userType)
    return createError(ERROR_TYPE::TYPE,
                       "attribute access requires a user-defined type");

  auto attrRes = userType->getAttribute(node->id());
  if (!attrRes)
    return createError(attrRes.error());
  auto attrType = attrRes.value().type();
  /*
  if (insideDeclWithGenerics &&
      typeTable_->isGenericType(attrType, currentGenericList_))
    attrType = std::make_shared<PlaceHolder>(attrType);
  */
  currentType_ = attrType;
  node->setReturnedFromAnalysis(attrType);
  return attrType;
}

/*
- tener en cuenta generics
- se apoya en currentType para saber si el metodo existe y retorna el tipo
- igual que funcion
*/
std::expected<std::shared_ptr<Type>, Error>
TypeAnalysis::visit(const AST_METHOD_CALL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_METHOD_CALL");
  }

  std::vector<std::shared_ptr<Type>> argTypes;
  for (const auto &expr : node->parameters()) {
    auto res = expr->accept(*this);
    if (!res)
      return createError(res.error());
    argTypes.push_back(res.value());
  }

  if (!currentType_)
    return createError(ERROR_TYPE::TYPE,
                       "current type not set for method call");

  auto userType = std::dynamic_pointer_cast<UserType>(currentType_);
  if (!userType)
    return createError(ERROR_TYPE::TYPE,
                       "method call requires a user-defined type");

  auto methodsExp = userType->getMethods(node->id());
  if (!methodsExp)
    return createError(methodsExp.error());
  auto methods = methodsExp.value();
  if (methods.empty())
    return createError(ERROR_TYPE::METHOD,
                       "no method with id: " + node->id() + " exists");

  std::vector<Method> viableMethods;
  const auto &explicitGenerics = node->replaceOfGenerics();

  for (const auto &method : methods) {
    // Check if method generics match the explicit generics, if provided.
    if (!explicitGenerics.empty() &&
        method.generics().size() != explicitGenerics.size())
      continue;
    // Check parameter count.
    if (method.params().size() != argTypes.size())
      continue;
    bool candidateMatches = true;
    const auto &params = method.params().params();
    for (size_t i = 0; i < params.size(); ++i) {
      auto paramType = params[i].second;
      auto argType = argTypes[i];
      // For generics, if either the parameter or argument type is still
      // generic, we skip the strict check here.
      if (insideDeclWithGenerics &&
          (typeTable_->isGenericType(paramType, currentGenericList_) ||
           typeTable_->isGenericType(argType, currentGenericList_))) {
        continue;
      }
      if (!typeTable_->canAssign(paramType, argType)) {
        candidateMatches = false;
        break;
      }
    }
    if (candidateMatches)
      viableMethods.push_back(method);
  }

  if (viableMethods.empty())
    return createError(ERROR_TYPE::METHOD,
                       "no matching method found for call: " + node->id());
  if (viableMethods.size() > 1)
    return createError(ERROR_TYPE::METHOD,
                       "ambiguous method call for: " + node->id());
  const auto returnType{viableMethods.front().returnType()};
  node->setReturnedFromAnalysis(returnType);
  return returnType;
}

/*
- igual que funcion
*/
std::expected<std::shared_ptr<Type>, Error>
TypeAnalysis::visit(const AST_METHOD_DECL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_METHOD_DECL");
  }
  if (!node->generics().empty()) {
    insideDeclWithGenerics = true;
    currentGenericList_ =
        mergeGenericLists(currentStructGenericList_, node->generics());
  }

  auto bodyRes = node->body()->accept(*this);
  if (!bodyRes)
    return createError(bodyRes.error());
  auto bodyType = bodyRes.value();

  if (!typeTable_->areSameType(bodyType, typeTable_->noPropagateType()) &&
      !typeTable_->areSameType(bodyType, typeTable_->breakType())) {
    if (!typeTable_->canAssign(node->returnType(), bodyType))
      return createError(
          ERROR_TYPE::TYPE,
          "method body return type does not match declared return type");
  }

  insideDeclWithGenerics = false;
  currentGenericList_.clear();
  node->setReturnedFromAnalysis(typeTable_->noPropagateType());
  return typeTable_->noPropagateType();
}

/*
- igual que funcion
*/
std::expected<std::shared_ptr<Type>, Error>
TypeAnalysis::visit(const AST_CONSTRUCTOR_DECL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_CONSTRUCTOR_DECL");
  }

  if (!node->generics().empty()) {
    insideDeclWithGenerics = true;
    currentGenericList_ =
        mergeGenericLists(currentStructGenericList_, node->generics());
  }

  auto bodyRes = node->body()->accept(*this);
  if (!bodyRes)
    return createError(bodyRes.error());
  auto bodyType = bodyRes.value();

  if (!typeTable_->areSameType(bodyType, typeTable_->noPropagateType()) &&
      !typeTable_->areSameType(bodyType, typeTable_->breakType()))
    return createError(ERROR_TYPE::TYPE,
                       "constructor body must not return a value");

  insideDeclWithGenerics = false;
  currentGenericList_.clear();
  node->setReturnedFromAnalysis(typeTable_->noPropagateType());
  return typeTable_->noPropagateType();
}

/*
- retorna el tipo del padre
- comprobar generics y argumentos
*/
std::expected<std::shared_ptr<Type>, Error>
TypeAnalysis::visit(const AST_SUPER *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_SUPER");
  }
  for (const auto &arg : node->arguments()) {
    auto res = arg->accept(*this);
    if (!res)
      return createError(res.error());
  }

  std::vector<std::shared_ptr<Type>> processedReplacements;
  for (const auto &rep : node->replacements()) {
    std::shared_ptr<Type> proc = rep;
    /*
    if (insideDeclWithGenerics &&
        typeTable_->isGenericType(rep, currentGenericList_))
      proc = std::make_shared<PlaceHolder>(rep);
    */
    processedReplacements.push_back(proc);
  }

  auto fatherType = node->fatherType();
  if (!fatherType)
    return createError(ERROR_TYPE::TYPE,
                       "father type not specified in super call");

  if (!processedReplacements.empty()) {
    auto userType = std::dynamic_pointer_cast<UserType>(fatherType);
    if (!userType)
      return createError(
          ERROR_TYPE::TYPE,
          "father type in super call must be a user-defined type");
    const auto instance{
        std::make_shared<GenericInstanceType>(userType, processedReplacements)};
    node->setReturnedFromAnalysis(instance);
    return instance;
  }
  const auto result{typeTable_->getType(fatherType->toString())};
  if (!result) {
    return createError(result.error());
  }
  node->setReturnedFromAnalysis(*result);
  return result;
}

/*
- comprobar body
- retorna NoPropagate
*/
std::expected<std::shared_ptr<Type>, Error>
TypeAnalysis::visit(const AST_DESTRUCTOR_DECL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_DESTRUCTOR_DECL");
  }
  const auto body{node->body()->accept(*this)};
  if (!body) {
    return createError(body.error());
  }
  node->setReturnedFromAnalysis(typeTable_->noPropagateType());
  return typeTable_->noPropagateType();
}

/*
- retorna el tipo de la clase envuelto en un puntero const?
*/
std::expected<std::shared_ptr<Type>, Error>
TypeAnalysis::visit(const AST_THIS *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_THIS");
  }
  if (!node->userType())
    return createError(ERROR_TYPE::TYPE, "user type not set for 'this'");

  auto typeFromTable = typeTable_->getType(node->userType()->name());
  if (!typeFromTable)
    return createError(typeFromTable.error());
  auto userType = std::dynamic_pointer_cast<UserType>(*typeFromTable);
  if (!userType)
    return createError(ERROR_TYPE::TYPE,
                       "'this' must refer to a user-defined type");

  auto constType =
      std::make_shared<ConstType>(std::make_shared<PointerType>(userType));
  node->setReturnedFromAnalysis(constType);
  return std::make_shared<ConstType>(constType);
}

/*
- comprueba generics
- comprueba argumentos
- comprueba body
*/
std::expected<std::shared_ptr<Type>, Error>
TypeAnalysis::visit(const AST_CONSTRUCTOR_CALL *node) const noexcept {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_CONSTRUCTOR_CALL");

  auto tempUserType = std::make_shared<UserType>(
      node->id(), nullptr, std::vector<GenericParameter>{});
  if (insideDeclWithGenerics &&
      typeTable_->isGenericType(tempUserType, currentGenericList_)) {
    if (!node->replaceOfGenerics().empty()) {
      std::vector<std::shared_ptr<Type>> genericArgs;
      for (const auto &gen : node->replaceOfGenerics()) {
        if (insideDeclWithGenerics &&
            typeTable_->isGenericType(gen, currentGenericList_)) {
           genericArgs.push_back(gen);
        } else
          genericArgs.push_back(gen);
      }
      return std::make_shared<GenericInstanceType>(tempUserType, genericArgs);
    } else {
      // No se proporcionaron reemplazos; devolvemos un PlaceHolder para el tipo
      // genérico.
      return tempUserType;
    }
  }

  std::vector<std::shared_ptr<Type>> argTypes;
  for (const auto &expr : node->parameters()) {
    auto res = expr->accept(*this);
    if (!res)
      return createError(res.error());
    argTypes.push_back(res.value());
  }

  auto baseTypeExp = typeTable_->getType(node->id());
  if (!baseTypeExp)
    return createError(baseTypeExp.error());
  auto baseType = baseTypeExp.value();

  if (const auto enumType{std::dynamic_pointer_cast<EnumType>(baseType)}) {
    if (argTypes.size() != 1) {
      return createError(
          ERROR_TYPE::TYPE,
          "constructor call of a enum must have one argument of the same type");
    }
    if (!typeTable_->canAssign(enumType, argTypes[0])) {
      return createError(
          ERROR_TYPE::TYPE,
          "constructor call of a enum must have one argument of the same type");
    }
    return enumType;
  }

  auto userType = std::dynamic_pointer_cast<UserType>(baseType);
  if (!userType)
    return createError(ERROR_TYPE::TYPE,
                       "constructor call id must refer to a user-defined type");

  auto cons = userType->constructor();
  if (!cons)
    return createError(ERROR_TYPE::FUNCTION,
                       "no constructor defined for type: " + userType->name());

  if (cons->params().size() != argTypes.size())
    return createError(ERROR_TYPE::FUNCTION,
                       "constructor parameter count mismatch for type: " +
                           userType->name());

  const auto &consParams = cons->params().params();
  for (size_t i = 0; i < consParams.size(); ++i) {
    auto paramType = consParams[i].second;
    auto argType = argTypes[i];
    if (!typeTable_->canAssign(paramType, argType))
      return createError(ERROR_TYPE::TYPE,
                         "argument " + std::to_string(i) +
                             " type mismatch in constructor call for type: " +
                             userType->name());
  }

  if (!node->replaceOfGenerics().empty()) {
    std::vector<std::shared_ptr<Type>> genericArgs;
    for (const auto &gen : node->replaceOfGenerics()) {
      if (insideDeclWithGenerics &&
          typeTable_->isGenericType(gen, currentGenericList_)) {
         genericArgs.push_back(gen);
      } else
        genericArgs.push_back(gen);
    }
    const auto instance{
        std::make_shared<GenericInstanceType>(userType, genericArgs)};
    node->setReturnedFromAnalysis(instance);
    return instance;
  }
  node->setReturnedFromAnalysis(baseType);
  return baseType;
}

} // namespace nicole