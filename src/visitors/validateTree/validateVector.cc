#include "../../../inc/parsingAnalysis/ast/vector/ast_index.h"
#include "../../../inc/parsingAnalysis/ast/vector/ast_vector.h"
#include "../../../inc/parsingAnalysis/checkPosition.h"
#include "../../../inc/visitors/validateTree/validateTree.h"

namespace nicole {

auto ValidateTree::visit(const AST_VECTOR *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_VECTOR");
  }
  if (CheckPosition::hasEveryAncestorInOrder(
          node, {AST_TYPE::STATEMENT, AST_TYPE::BODY})) {
    return createError(ERROR_TYPE::VALIDATE_TREE, "dangling vector");
  }
  for (const auto &value : node->values()) {
    const auto result{value->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return true;
}

// chained
auto ValidateTree::visit(const AST_INDEX *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_INDEX");
  }
  if (!CheckPosition::hasAnyAncestorOf(node, {AST_TYPE::CHAIN})) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "index operator can only appear in a chain expression");
  }
  const auto result{node->index()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return true;
}

} // namespace nicole