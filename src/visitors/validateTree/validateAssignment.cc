#include "../../../inc/parsingAnalysis/ast/assignments/ast_assignment.h"
#include "../../../inc/parsingAnalysis/checkPosition.h"
#include "../../../inc/visitors/validateTree/validateTree.h"

namespace nicole {

// statemetn / body / not null or for
auto ValidateTree::visit(const AST_ASSIGNMENT *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ASSIGNMENT");
  }
  if (!(CheckPosition::itsBodyAncestorHasParent(node) or
        CheckPosition::isInsideForHeader(node))) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a assignment can only exist "
                       "in a body or a for header init");
  }
  const auto left{node->left()->accept(*this)};
  if (!left) {
    return createError(left.error());
  }
  const auto right{node->value()->accept(*this)};
  if (!right) {
    return createError(right.error());
  }
  return true;
}

} // namespace nicole