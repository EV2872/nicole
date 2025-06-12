#include "../../../inc/parsingAnalysis/ast/operators/ast_binary.h"
#include "../../../inc/parsingAnalysis/ast/operators/ast_unary.h"
#include "../../../inc/parsingAnalysis/checkPosition.h"
#include "../../../inc/visitors/validateTree/validateTree.h"

namespace nicole {

auto ValidateTree::visit(const AST_BINARY *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ADD");
  }
  if (CheckPosition::isOutOfScope(node)) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       node->op().raw() +
                           " operation cannot appear outside of a scope, at " +
                           node->op().locInfo());
  }
  const auto left{node->left()->accept(*this)};
  if (!left) {
    return createError(left.error());
  }
  const auto right{node->right()->accept(*this)};
  if (!right) {
    return createError(right.error());
  }
  return true;
}

auto ValidateTree::visit(const AST_UNARY *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_NEG");
  }
  if (CheckPosition::isOutOfScope(node)) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       node->op().raw() +
                           " operation cannot appear outside of a scope");
  }
  const auto left{node->value()->accept(*this)};
  if (!left) {
    return createError(left.error());
  }
  return true;
}

} // namespace nicole