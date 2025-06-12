#include "../../../inc/parsingAnalysis/ast/utils/ast_import.h"
#include "../../../inc/parsingAnalysis/ast/utils/ast_print.h"
#include "../../../inc/visitors/typeAnalysis/typeAnalysis.h"
#include <memory>

namespace nicole {

/*
- comprueba que los tipos de los argumentos no sean void ni NoPropagate,
BreakType
- si es un userType comprueba que exista el metodo toStr()
- retorna NoPropagate
 */
auto TypeAnalysis::visit(const AST_PRINT *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_PRINT");
  }
  for (const auto &expr : node->values()) {
    const auto result{expr->accept(*this)};
    if (!result) {
      return createError(result.error());
    }

    auto exprType = result.value();

    if (insideDeclWithGenerics &&
        typeTable_->isGenericType(exprType, currentGenericList_))
      continue;

    if (auto constType = std::dynamic_pointer_cast<ConstType>(exprType)) {
      exprType = constType->baseType();
    }
    if (auto vecType = std::dynamic_pointer_cast<VectorType>(exprType)) {
      return createError(ERROR_TYPE::TYPE, "cannot print a vector");
    }
    if (auto userType = std::dynamic_pointer_cast<UserType>(exprType)) {
      const auto exists = typeTable_->getType(userType->name());
      if (!exists) {
        return createError(exists.error());
      }
      auto methodsExp =
          std::dynamic_pointer_cast<UserType>(*exists)->getMethods("toString");
      if (!methodsExp) {
        return createError(methodsExp.error());
      }
      if (methodsExp.value().empty())
        return createError(ERROR_TYPE::TYPE, "User type " + userType->name() +
                                                 " must implement toString()");
    }
  }
  node->setReturnedFromAnalysis(typeTable_->noPropagateType());
  return typeTable_->noPropagateType();
}

/*
- retorna NoPropagate
*/
auto TypeAnalysis::visit(const AST_IMPORT *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_IMPORT");
  }
  node->setReturnedFromAnalysis(typeTable_->noPropagateType());
  return typeTable_->noPropagateType();
}

} // namespace nicole