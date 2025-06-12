#include "../../../inc/parsingAnalysis/ast/operators/ast_binary.h"
#include "../../../inc/parsingAnalysis/ast/operators/ast_unary.h"
#include "../../../inc/visitors/typeAnalysis/typeAnalysis.h"
#include <memory>

namespace nicole {

/*
- comrpueba que left no sea void ni NoPropagate, BreakType
- comrpueba que right no sea void ni NoPropagate, BreakType
- comrprueba que se puedan operar, caso speciales si un userType busca metodo
para sobrecarga de operador
*/
auto TypeAnalysis::visit(const AST_BINARY *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_BINARY");
  }
  auto leftRes = node->left()->accept(*this);
  if (!leftRes)
    return createError(leftRes.error());
  auto leftType = leftRes.value();

  auto rightRes = node->right()->accept(*this);
  if (!rightRes)
    return createError(rightRes.error());
  auto rightType = rightRes.value();

  TokenType op =
      node->op().type(); // Assumes AST_BINARY::op() returns a TokenType

  auto resultExp = typeTable_->applyBinaryOperator(leftType, rightType, op);
  if (!resultExp)
    return createError(resultExp.error());
  node->setReturnedFromAnalysis(*resultExp);
  return resultExp.value();
}

/*
- comprueba que expression no es void ni NoPropagate, BreakType
- comrpueba que se puede operar, caso speciales si un userType busca metodo
para sobrecarga de operador y retorna el tipo
 */
auto TypeAnalysis::visit(const AST_UNARY *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_UNARY");
  }

  auto operandRes = node->value()->accept(*this);
  if (!operandRes)
    return createError(operandRes.error());
  auto operandType = operandRes.value();
  TokenType op =
      node->op().type(); // Se asume que node->op() retorna un TokenType

  auto resultTypeExp = typeTable_->applyUnaryOperator(operandType, op);
  if (!resultTypeExp)
    return createError(resultTypeExp.error());
  node->setReturnedFromAnalysis(*resultTypeExp);
  return resultTypeExp.value();
}

} // namespace nicole