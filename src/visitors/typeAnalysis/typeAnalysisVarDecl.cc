#include "../../../inc/parsingAnalysis/ast/variables/ast_autoDecl.h"
#include "../../../inc/parsingAnalysis/ast/variables/ast_typedDecl.h"
#include "../../../inc/parsingAnalysis/ast/variables/ast_varCall.h"
#include "../../../inc/visitors/typeAnalysis/typeAnalysis.h"
#include <memory>

namespace nicole {

/*
- comrpueba que el tipo de la expresion no sea void
- fija en currentScope el tipo
- retorna NoPropagate
*/
std::expected<std::shared_ptr<Type>, Error>
TypeAnalysis::visit(const AST_AUTO_DECL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_AUTO_DECL");
  }
  auto exprTypeRes = node->value()->accept(*this);
  if (!exprTypeRes)
    return createError(exprTypeRes.error());
  auto deducedType = exprTypeRes.value();
  /*
  if (insideDeclWithGenerics &&
      typeTable_->isGenericType(deducedType, currentGenericList_))
    deducedType = std::make_shared<PlaceHolder>(deducedType);
  */
  auto insertRes = currentScope_->setVariableType(node->id(), deducedType);
  if (!insertRes)
    return createError(insertRes.error());

  if (auto ut = std::dynamic_pointer_cast<UserType>(deducedType)) {
    const auto variable{currentScope_->getVariable(node->id()).value()};
    currentScope_->registerForDestruction(variable);
  }

  node->setReturnedFromAnalysis(typeTable_->noPropagateType());
  return typeTable_->noPropagateType();
}

/*
- comrpueba que el tipo de la expresion no sea body
- comrpueba que la expresion sea compatible
- retorna NoPropagate
*/
std::expected<std::shared_ptr<Type>, Error>
TypeAnalysis::visit(const AST_VAR_TYPED_DECL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_VAR_TYPED_DECL");
  }
  auto declaredType = node->varType();
  /*
  if (insideDeclWithGenerics &&
      typeTable_->isGenericType(declaredType, currentGenericList_))
    declaredType = std::make_shared<PlaceHolder>(declaredType);
  */
  auto valueRes = node->value()->accept(*this);
  if (!valueRes)
    return createError(valueRes.error());
  auto valueType = valueRes.value();

  if (!typeTable_->canAssign(declaredType, valueType))
    return createError(
        ERROR_TYPE::TYPE,
        "assigned value type does not match declared variable type -> " +
            declaredType->toString() + " | " + valueType->toString());

  if (auto ut = std::dynamic_pointer_cast<UserType>(declaredType)) {
    const auto variable{currentScope_->getVariable(node->id()).value()};
    currentScope_->registerForDestruction(variable);
  }

  node->setReturnedFromAnalysis(typeTable_->noPropagateType());
  return typeTable_->noPropagateType();
}

/*
- se apoya en currentScope y en en currenStruct para saber si es un atributo
*/
std::expected<std::shared_ptr<Type>, Error>
TypeAnalysis::visit(const AST_VAR_CALL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_VAR_CALL");
  }

  if (analyzingInsideClass && currentUserType_ &&
      currentUserType_->hasAttribute(node->id())) {
    auto attrExp = currentUserType_->getAttribute(node->id());
    if (!attrExp)
      return createError(attrExp.error());
    node->setReturnedFromAnalysis(attrExp.value().type());
    return attrExp.value().type();
  }
  if (!currentScope_->has(node->id()))
    return createError(ERROR_TYPE::VARIABLE,
                       "variable: " + node->id() + " does not exist");
  auto varExp = currentScope_->getVariable(node->id());
  if (!varExp)
    return createError(varExp.error());
  currentType_ = (*varExp)->type();
  node->setReturnedFromAnalysis((*varExp)->type());
  return (*varExp)->type();
}

} // namespace nicole