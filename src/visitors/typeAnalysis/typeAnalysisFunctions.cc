#include "../../../inc/parsingAnalysis/ast/functions/ast_funcCall.h"
#include "../../../inc/parsingAnalysis/ast/functions/ast_funcDecl.h"
#include "../../../inc/parsingAnalysis/ast/functions/ast_return.h"
#include "../../../inc/visitors/typeAnalysis/typeAnalysis.h"
#include <cstddef>
#include <memory>

namespace nicole {

/*
- numero correcto de generics en caso de tener
- parametros son del tipo correcto (no pueden ser void ni punteros a este)
- body debe retornar un tipo compatible al tipo de la funcion
- si no tiene generics debe existir una función con esos argumentos
- retorna tipo de función
*/
auto TypeAnalysis::visit(const AST_FUNC_CALL *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_FUNC_CALL");
  }

  std::vector<std::shared_ptr<Type>> argTypes;
  for (const auto &expr : node->parameters()) {
    auto res = expr->accept(*this);
    if (!res)
      return createError(res.error());
    argTypes.push_back(res.value());
  }

  auto candidates = functionTable_->getFunctions(node->id());
  if (candidates.empty())
    return createError(ERROR_TYPE::FUNCTION,
                       "no function with id: " + node->id() + " exists");

  std::vector<Function> viableFunctions;
  const auto &explicitGenerics = node->replaceOfGenerics();

  for (const auto &func : candidates) {
    if (func.params().size() != argTypes.size())
      continue;
    if (!explicitGenerics.empty() &&
        func.generics().size() != explicitGenerics.size())
      continue;

    bool candidateMatches = true;
    const auto &funcParams = func.params().params();
    for (size_t i = 0; i < funcParams.size(); ++i) {
      auto paramType = funcParams[i].second;
      auto argType = argTypes[i];

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
      viableFunctions.push_back(func);
  }

  if (viableFunctions.empty())
    return createError(ERROR_TYPE::FUNCTION,
                       "no matching function found for call: " + node->id());
  if (viableFunctions.size() > 1)
    return createError(ERROR_TYPE::FUNCTION,
                       "ambiguous function call for: " + node->id());
  currentType_ = viableFunctions.front().returnType();
  const auto returnType{viableFunctions.front().returnType()};
  node->setReturnedFromAnalysis(returnType);
  node->setDeclReference(viableFunctions.front().declReference());
  return returnType;
}

/*
- parametros no pueden ser void ni punteros a este
- comprobar si algo es generic
- body debe retornar un tipo compatible al tipo de la funcion
- retorna el tipo NoPropagate
*/
auto TypeAnalysis::visit(const AST_FUNC_DECL *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_FUNC_DECL");

  firstBody = false;

  if (!node->generics().empty()) {
    insideDeclWithGenerics = true;
    currentGenericList_ = node->generics();
  }

  auto bodyRes = node->body()->accept(*this);
  if (!bodyRes)
    return createError(bodyRes.error());
  auto bodyType = bodyRes.value();
  auto voidType = typeTable_->voidType();
  auto declaredReturnType = node->returnType();

  if (typeTable_->areSameType(bodyType, typeTable_->noPropagateType()) ||
      typeTable_->areSameType(bodyType, voidType)) {
    if (!typeTable_->areSameType(declaredReturnType, voidType)) {
      return createError(ERROR_TYPE::TYPE,
                         "function body returns void/noPropagate, but function "
                         "return type is: " +
                             declaredReturnType->toString());
    }
  } else {
    if (!typeTable_->canAssign(declaredReturnType, bodyType)) {
      return createError(
          ERROR_TYPE::TYPE,
          "function body return type does not match declared return type -> " +
              declaredReturnType->toString() + " | " + bodyType->toString());
    }
  }

  insideDeclWithGenerics = false;
  firstBody = true;
  currentGenericList_.clear();
  node->setReturnedFromAnalysis(typeTable_->noPropagateType());
  return typeTable_->noPropagateType();
}

/*
- si esta vacio retorna void
- si no el tipo de la expresion
*/
auto TypeAnalysis::visit(const AST_RETURN *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_RETURN");
  }

  if (firstBody)
    foundReturn = true;
  else {
    foundReturn = false;
  }
  const auto voidType{typeTable_->voidType()};
  if (!node->expression()) {
    node->setReturnedFromAnalysis(voidType);
    return voidType;
  }
  auto result = node->expression()->accept(*this);
  if (!result)
    return createError(result.error());
  auto retType = result.value();

  if (insideDeclWithGenerics &&
      typeTable_->isGenericType(retType, currentGenericList_)) {
    node->setReturnedFromAnalysis(retType);
    return retType;
  }

  if (typeTable_->areSameType(retType, voidType)) {
    return createError(ERROR_TYPE::TYPE,
                       "a return statement can return void when it does not "
                       "have expression, but if it has it must not be void");
  }

  node->setReturnedFromAnalysis(retType);
  return retType;
}

} // namespace nicole