#include "../../../inc/parsingAnalysis/ast/enum/ast_enum.h"
#include "../../../inc/parsingAnalysis/ast/enum/ast_enumAccess.h"
#include "../../../inc/parsingAnalysis/checkPosition.h"
#include "../../../inc/visitors/validateTree/validateTree.h"

namespace nicole {

// statement / body / null
auto ValidateTree::visit(const AST_ENUM *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ENUM");
  }
  if (CheckPosition::itsBodyAncestorHasParent(node)) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a enum declaration must be outside of any scope");
  }
  return true;
}

auto ValidateTree::visit(const AST_ENUM_ACCESS *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ENUM_ACCESS");
  }
  if (CheckPosition::hasEveryAncestorInOrder(
          node, {AST_TYPE::STATEMENT, AST_TYPE::BODY})) {
    return createError(ERROR_TYPE::VALIDATE_TREE, "dangling enum access");
  }
  return true;
}

} // namespace nicole