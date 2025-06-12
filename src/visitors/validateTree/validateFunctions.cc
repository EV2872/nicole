#include "../../../inc/parsingAnalysis/ast/functions/ast_funcCall.h"
#include "../../../inc/parsingAnalysis/ast/functions/ast_funcDecl.h"
#include "../../../inc/parsingAnalysis/ast/functions/ast_return.h"
#include "../../../inc/parsingAnalysis/checkPosition.h"
#include "../../../inc/visitors/validateTree/validateTree.h"

namespace nicole {

// chained
auto ValidateTree::visit(const AST_FUNC_CALL *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_FUNC_CALL");
  }
  if (node->father() and node->father()->type() != AST_TYPE::CHAIN) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "invalid hierachy AST_FUNC_CALL");
  }
  for (const auto &chain : node->parameters()) {
    const auto result{chain->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return true;
}

// statement / body / null or struct / class
auto ValidateTree::visit(const AST_FUNC_DECL *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_FUNC_DECL");
  }
  if (CheckPosition::itsBodyAncestorHasParent(node)) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a funciton declaration cant appear inside a scope: " +
                           node->id());
  }
  const auto result{node->body()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return true;
}

// func decl
auto ValidateTree::visit(const AST_RETURN *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_RETURN");
  }
  if (!CheckPosition::hasAnyAncestorOf(
          node, {AST_TYPE::FUNC_DECL, AST_TYPE::METHOD_DECL})) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a return declaration must has a function "
                       "declaration in its hierarchy");
  }
  const auto result{node->expression()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return true;
}

} // namespace nicole