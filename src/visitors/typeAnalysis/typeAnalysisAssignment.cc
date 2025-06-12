#include "../../../inc/parsingAnalysis/ast/assignments/ast_assignment.h"
#include "../../../inc/visitors/typeAnalysis/typeAnalysis.h"
#include <memory>

namespace nicole {

/*
- comprueba que left no sea void ni NoPropagate, BreakType
- comrpueba que right no sea void ni NoPropagate, BreakType
- comprueba que sea asignable
- retorna NoPropagate
*/
auto TypeAnalysis::visit(const AST_ASSIGNMENT *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ASSIGNMENT");
  }
  const auto left{node->left()->accept(*this)};
  if (!left) {
    return createError(left.error());
  }
  if (const auto constType{std::dynamic_pointer_cast<ConstType>(*left)}) {
    return createError(ERROR_TYPE::TYPE, "cannot reassign to a const variable");
  }
  const auto right{node->value()->accept(*this)};
  if (!right) {
    return createError(right.error());
  }
  if (!typeTable_->canAssign(left.value(), right.value()))
    return createError(ERROR_TYPE::TYPE, "incompatible types in assignment");
  node->setReturnedFromAnalysis(typeTable_->noPropagateType());
  return typeTable_->noPropagateType();
}

} // namespace nicole