#include "../../../inc/parsingAnalysis/ast/conditionals/ast_case.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_default.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_elseIf.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_if.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_switch.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_ternary.h"
#include "../../../inc/visitors/fillSemanticInfo/fillSemanticInfo.h"
#include <variant>

namespace nicole {

auto FillSemanticInfo::visit(const AST_IF *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_IF");
  }
  const auto condition{node->condition()->accept(*this)};
  if (!condition) {
    return createError(condition.error());
  }
  pushScope();
  node->body()->setScope(currentScope_);
  const auto body{node->body()->accept(*this)};
  if (!body) {
    return createError(body.error());
  }
  popScope();
  for (const auto &elseIf : node->elseIf()) {
    const auto result{elseIf->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  if (node->elseBody()) {
    pushScope();
    node->elseBody()->setScope(currentScope_);
    const auto elseBody{node->elseBody()->accept(*this)};
    if (!elseBody) {
      return createError(elseBody.error());
    }
    popScope();
  }
  return {};
}

auto FillSemanticInfo::visit(const AST_ELSE_IF *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ELSE_IF");
  }
  const auto condition{node->condition()->accept(*this)};
  if (!condition) {
    return createError(condition.error());
  }
  pushScope();
  node->body()->setScope(currentScope_);
  const auto body{node->body()->accept(*this)};
  if (!body) {
    return createError(body.error());
  }
  popScope();
  return {};
}

auto FillSemanticInfo::visit(const AST_SWITCH *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_SWITCH");
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
  return {};
}

auto FillSemanticInfo::visit(const AST_CASE *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_CASE");
  }
  const auto match{node->match()->accept(*this)};
  if (!match) {
    return createError(match.error());
  }
  pushScope();
  node->body()->setScope(currentScope_);
  const auto body{node->body()->accept(*this)};
  if (!body) {
    return createError(body.error());
  }
  popScope();
  return {};
}

auto FillSemanticInfo::visit(const AST_DEFAULT *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_DEFAULT");
  }
  pushScope();
  node->body()->setScope(currentScope_);
  const auto body{node->body()->accept(*this)};
  if (!body) {
    return createError(body.error());
  }
  popScope();
  return {};
}

auto FillSemanticInfo::visit(const AST_TERNARY *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_TERNARY");
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
  return {};
}

auto FillSemanticInfo::visit(const AST_CONDITION *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_CONDITION");
  }
  return node->condition()->accept(*this);
}

} // namespace nicole