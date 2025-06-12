#include "../../../inc/parsingAnalysis/ast/loops/ast_doWhile.h"
#include "../../../inc/parsingAnalysis/ast/loops/ast_for.h"
#include "../../../inc/parsingAnalysis/ast/loops/ast_pass.h"
#include "../../../inc/parsingAnalysis/ast/loops/ast_stop.h"
#include "../../../inc/parsingAnalysis/ast/loops/ast_while.h"
#include "../../../inc/visitors/fillSemanticInfo/fillSemanticInfo.h"
#include <variant>

namespace nicole {

auto FillSemanticInfo::visit(const AST_WHILE *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_WHILE");
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

auto FillSemanticInfo::visit(const AST_FOR *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_FOR");
  }

  for (const auto &expr : node->init()) {
    const auto result{expr->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  pushScope();
  node->body()->setScope(currentScope_);
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
  const auto body{node->body()->accept(*this)};
  if (!body) {
    return createError(body.error());
  }
  popScope();
  return {};
}

auto FillSemanticInfo::visit(const AST_DO_WHILE *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_DO_WHILE");
  }
  pushScope();
  node->body()->setScope(currentScope_);
  const auto body{node->body()->accept(*this)};
  if (!body) {
    return createError(body.error());
  }
  popScope();
  const auto condition{node->condition()->accept(*this)};
  if (!condition) {
    return createError(condition.error());
  }
  return {};
}

auto FillSemanticInfo::visit(const AST_PASS *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_PASS");
  }
  return {};
}

auto FillSemanticInfo::visit(const AST_STOP *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_STOP");
  }
  return {};
}

} // namespace nicole