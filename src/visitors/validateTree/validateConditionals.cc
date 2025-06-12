#include "../../../inc/parsingAnalysis/ast/conditionals/ast_case.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_default.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_elseIf.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_if.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_switch.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_ternary.h"
#include "../../../inc/parsingAnalysis/checkPosition.h"
#include "../../../inc/visitors/validateTree/validateTree.h"

namespace nicole {

// statement / body / not null
auto ValidateTree::visit(const AST_IF *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_IF");
  }
  if (!CheckPosition::itsBodyAncestorHasParent(node)) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a if statement must appear inside of a scope");
  }
  const auto condition{node->condition()->accept(*this)};
  if (!condition) {
    return createError(condition.error());
  }
  const auto body{node->body()->accept(*this)};
  if (!body) {
    return createError(body.error());
  }
  for (const auto &elseif_ : node->elseIf()) {
    const auto result{elseif_->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  if (node->elseBody()) {
    const auto elseBody{node->elseBody()->accept(*this)};
    if (!elseBody) {
      return createError(elseBody.error());
    }
  }
  return true;
}

// if
auto ValidateTree::visit(const AST_ELSE_IF *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ELSE_IF");
  }
  if (!CheckPosition::hasEveryAncestorInOrder(node, {AST_TYPE::IF})) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a else if must follow a if statement");
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
auto ValidateTree::visit(const AST_SWITCH *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_SWITCH");
  }
  if (!CheckPosition::itsBodyAncestorHasParent(node)) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a switch statement must appear inside of a scope");
  }
  const auto condition{node->condition()->accept(*this)};
  if (!condition) {
    return createError(condition.error());
  }
  for (const auto &case_ : node->cases()) {
    const auto result{case_->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  if (node->defaultCase()) {
    const auto defaultCase{node->defaultCase()->accept(*this)};
    if (!defaultCase) {
      return createError(defaultCase.error());
    }
  }
  return true;
}

// switch
auto ValidateTree::visit(const AST_CASE *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_CASE");
  }
  if (!CheckPosition::hasEveryAncestorInOrder(node, {AST_TYPE::SWITCH})) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a case statement can only appear in a switch scope");
  }
  const auto match{node->match()->accept(*this)};
  if (!match) {
    return createError(match.error());
  }
  const auto result{node->body()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return true;
}

// switch
auto ValidateTree::visit(const AST_DEFAULT *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_DEFAULT");
  }
  if (!CheckPosition::hasEveryAncestorInOrder(node, {AST_TYPE::SWITCH})) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a default statement can only appear in a switch scope");
  }
  const auto result{node->body()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return true;
}

auto ValidateTree::visit(const AST_TERNARY *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_TERNARY");
  }
  if (CheckPosition::isOutOfScope(node)) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a ternary operation cannot appear outside of a scope");
  }
  const auto condition{node->condition()->accept(*this)};
  if (!condition) {
    return createError(condition.error());
  }
  const auto first{node->first()->accept(*this)};
  if (!first) {
    return createError(first.error());
  }
  const auto second{node->second()->accept(*this)};
  if (!second) {
    return createError(second.error());
  }
  return true;
}

auto ValidateTree::visit(const AST_CONDITION *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_CONDITION");
  }
  if (!CheckPosition::hasAnyAncestorOf(
          node, {AST_TYPE::IF, AST_TYPE::ELSE_IF, AST_TYPE::SWITCH,
                 AST_TYPE::DO_WHILE, AST_TYPE::WHILE, AST_TYPE::FOR})) {
    return createError(ERROR_TYPE::VALIDATE_TREE, "misplaced condition");
  }
  return node->condition()->accept(*this);
}

} // namespace nicole