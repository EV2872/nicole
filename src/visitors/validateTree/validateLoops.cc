#include "../../../inc/parsingAnalysis/ast/loops/ast_doWhile.h"
#include "../../../inc/parsingAnalysis/ast/loops/ast_for.h"
#include "../../../inc/parsingAnalysis/ast/loops/ast_pass.h"
#include "../../../inc/parsingAnalysis/ast/loops/ast_stop.h"
#include "../../../inc/parsingAnalysis/ast/loops/ast_while.h"
#include "../../../inc/parsingAnalysis/checkPosition.h"
#include "../../../inc/visitors/validateTree/validateTree.h"

namespace nicole {

// statement / body / not null
auto ValidateTree::visit(const AST_WHILE *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_WHILE");
  }
  if (!CheckPosition::itsBodyAncestorHasParent(node)) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a while loop must appear in a scope");
  }
  const auto condition{node->condition()->accept(*this)};
  if (!condition) {
    return createError(condition.error());
  }
  const auto result{node->body()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return true;
}

// statement / body / not null
auto ValidateTree::visit(const AST_FOR *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_FOR");
  }
  if (!CheckPosition::itsBodyAncestorHasParent(node)) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a for loop must appear in a scope");
  }
  for (const auto &expr : node->init()) {
    const auto result{expr->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  const auto condition{node->condition()->accept(*this)};
  if (!condition) {
    return createError(condition.error());
  }
  for (const auto &expr : node->update()) {
    const auto result{expr->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  const auto result{node->body()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return true;
}

// statement / body / not null
auto ValidateTree::visit(const AST_DO_WHILE *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_DO_WHILE");
  }
  if (!CheckPosition::itsBodyAncestorHasParent(node)) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a do while loop must appear in a scope");
  }
  const auto result{node->body()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  const auto condition{node->condition()->accept(*this)};
  if (!condition) {
    return createError(condition.error());
  }
  return true;
}

// do while / while / for
auto ValidateTree::visit(const AST_PASS *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_PASS");
  }
  if (!CheckPosition::hasAnyAncestorOf(
          node, {AST_TYPE::DO_WHILE, AST_TYPE::FOR, AST_TYPE::WHILE})) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a pass statement must appear inside a loop");
  }
  return true;
}

// do while / while / for
auto ValidateTree::visit(const AST_STOP *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_STOP");
  }
  if (!CheckPosition::hasAnyAncestorOf(node,
                                       {AST_TYPE::DO_WHILE, AST_TYPE::FOR,
                                        AST_TYPE::WHILE, AST_TYPE::SWITCH})) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a stop statement must appear inside a loop");
  }
  return true;
}

} // namespace nicole