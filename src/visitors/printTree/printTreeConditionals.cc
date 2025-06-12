#include "../../../inc/parsingAnalysis/ast/conditionals/ast_case.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_default.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_elseIf.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_if.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_switch.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_ternary.h"
#include "../../../inc/visitors/printTree/printTree.h"
#include <ostream>

namespace nicole {

auto PrintTree::visit(const AST_IF *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_IF");
  }
  std::ostringstream result;
  result << indent_ << "If Statement:\n";
  increaseIndent();
  const auto condition{node->condition()->accept(*this)};
  if (!condition) {
    return createError(condition.error());
  }
  result << *condition;

  for (const auto &statement : node->body()->body()) {
    const auto stmt{statement->accept(*this)};
    if (!stmt) {
      return createError(stmt.error());
    }
    result << *stmt;
  }

  if (node->elseIf().size() > 0) {
    result << indent_ << "Else Ifs:\n";
    for (const auto &elseIf : node->elseIf()) {
      const auto elseIfStr{elseIf->accept(*this)};
      if (!elseIfStr) {
        return createError(elseIfStr.error());
      }
      result << *elseIfStr;
    }
  }

  if (node->elseBody()) {
    result << indent_ << "Else:\n";
    for (const auto &statement : node->elseBody()->body()) {
      const auto stmt{statement->accept(*this)};
      if (!stmt) {
        return createError(stmt.error());
      }
      result << *stmt;
    }
  }

  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_ELSE_IF *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ELSE_IF");
  }
  std::ostringstream result;
  result << indent_ << "Else If:\n";
  increaseIndent();
  const auto condition{node->condition()->accept(*this)};
  if (!condition) {
    return createError(condition.error());
  }
  result << *condition;

  for (const auto &statement : node->body()->body()) {
    const auto stmt{statement->accept(*this)};
    if (!stmt) {
      return createError(stmt.error());
    }
    result << *stmt;
  }

  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_SWITCH *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_SWITCH");
  }
  std::ostringstream result;
  result << indent_ << "Switch Statement:\n";
  increaseIndent();
  const auto expr{node->condition()->accept(*this)};
  if (!expr) {
    return createError(expr.error());
  }
  result << *expr;

  result << indent_ << "Cases:\n";
  for (const auto &caseStmt : node->cases()) {
    const auto caseStr{caseStmt->accept(*this)};
    if (!caseStr) {
      return createError(caseStr.error());
    }
    result << *caseStr;
  }
  if (node->defaultCase()) {
    const auto defaultCase{node->defaultCase()->accept(*this)};
    if (!defaultCase) {
      return createError(defaultCase.error());
    }
    result << *defaultCase;
  }
  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_CASE *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_CASE");
  }
  std::ostringstream result;
  result << indent_ << "Case:\n";
  increaseIndent();
  const auto cond{node->match()->accept(*this)};
  if (!cond) {
    return createError(cond.error());
  }
  result << *cond;

  for (const auto &statement : node->body()->body()) {
    const auto stmt{statement->accept(*this)};
    if (!stmt) {
      return createError(stmt.error());
    }
    result << *stmt;
  }
  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_DEFAULT *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_DEFAULT");
  }
  std::ostringstream result;
  result << indent_ << "Default Case:\n";
  increaseIndent();
  for (const auto &statement : node->body()->body()) {
    const auto stmt{statement->accept(*this)};
    if (!stmt) {
      return createError(stmt.error());
    }
    result << *stmt;
  }
  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_TERNARY *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_TERNARY");
  }
  std::ostringstream result;
  result << indent_ << "Ternary Expression:\n";
  increaseIndent();
  const auto cond{node->condition()->accept(*this)};
  if (!cond) {
    return createError(cond.error());
  }
  result << *cond;

  result << indent_ << "True Branch:\n";
  const auto trueBranch{node->first()->accept(*this)};
  if (!trueBranch) {
    return createError(trueBranch.error());
  }
  result << *trueBranch;

  result << indent_ << "False Branch:\n";
  const auto falseBranch{node->second()->accept(*this)};
  if (!falseBranch) {
    return createError(falseBranch.error());
  }
  result << *falseBranch;
  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_CONDITION *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_CONDITION");
  }
  std::ostringstream result;
  result << indent_ << "Condition:\n";
  increaseIndent();
  const auto expr{node->condition()->accept(*this)};
  if (!expr) {
    return createError(expr.error());
  }
  result << *expr;
  decreaseIndent();
  return result.str();
}

} // namespace nicole