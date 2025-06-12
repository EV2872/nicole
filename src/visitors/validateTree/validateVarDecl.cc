#include "../../../inc/parsingAnalysis/ast/variables/ast_autoDecl.h"
#include "../../../inc/parsingAnalysis/ast/variables/ast_typedDecl.h"
#include "../../../inc/parsingAnalysis/ast/variables/ast_varCall.h"
#include "../../../inc/parsingAnalysis/checkPosition.h"
#include "../../../inc/visitors/validateTree/validateTree.h"

namespace nicole {

// statement / body / not null or for
auto ValidateTree::visit(const AST_AUTO_DECL *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_AUTO_DECL");
  }
  if (!(CheckPosition::itsBodyAncestorHasParent(node) or
        CheckPosition::isInsideForHeader(node))) {
    return createError(
        ERROR_TYPE::VALIDATE_TREE,
        "a auto declaration can only exist in a body or a for header init");
  }
  const auto result{node->value()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return true;
}

// statement / body / not null or for
auto ValidateTree::visit(const AST_VAR_TYPED_DECL *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_VAR_TYPED_DECL");
  }
  if (!(CheckPosition::itsBodyAncestorHasParent(node) or
        CheckPosition::isInsideForHeader(node))) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a var typed declaration can only exist in a body or a "
                       "for header init");
  }
  const auto result{node->value()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return true;
}

// chained
auto ValidateTree::visit(const AST_VAR_CALL *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_VAR_CALL");
  }
  if (node->father() and node->father()->type() != AST_TYPE::CHAIN) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "invalid hierachy AST_VAR_CALL");
  }
  return true;
}

} // namespace nicole