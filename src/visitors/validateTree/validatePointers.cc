#include "../../../inc/parsingAnalysis/ast/pointer/ast_delete.h"
#include "../../../inc/parsingAnalysis/ast/pointer/ast_deref.h"
#include "../../../inc/parsingAnalysis/ast/pointer/ast_new.h"
#include "../../../inc/parsingAnalysis/checkPosition.h"
#include "../../../inc/visitors/validateTree/validateTree.h"

namespace nicole {

// statemetn / body / not null
auto ValidateTree::visit(const AST_DELETE *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_DELETE");
  }
  if (!CheckPosition::itsBodyAncestorHasParent(node)) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a delete statement must appear in scope");
  }
  const auto result{node->value()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return true;
}

auto ValidateTree::visit(const AST_NEW *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_NEW");
  }
  if (CheckPosition::hasEveryAncestorInOrder(
          node, {AST_TYPE::STATEMENT, AST_TYPE::BODY})) {
    return createError(ERROR_TYPE::VALIDATE_TREE, "dangling new operator");
  }
  const auto result{node->value()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return true;
}

auto ValidateTree::visit(const AST_DEREF *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_DEREF");
  }
  if (CheckPosition::isOutOfScope(node)) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a deref operation cannot appear outside of a scope");
  }
  const auto result{node->value()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return true;
}

} // namespace nicole