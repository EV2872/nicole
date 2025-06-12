#include "../../../inc/parsingAnalysis/ast/loops/ast_doWhile.h"
#include "../../../inc/parsingAnalysis/ast/loops/ast_for.h"
#include "../../../inc/parsingAnalysis/ast/loops/ast_pass.h"
#include "../../../inc/parsingAnalysis/ast/loops/ast_stop.h"
#include "../../../inc/parsingAnalysis/ast/loops/ast_while.h"
#include "../../../inc/visitors/printTree/printTree.h"
#include <ostream>

namespace nicole {

auto PrintTree::visit(const AST_WHILE *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_WHILE");
  }
  std::ostringstream result;
  result << indent_ << "While Loop:\n";
  increaseIndent();

  increaseIndent();
  const auto condition{node->condition()->accept(*this)};
  if (!condition) {
    return createError(condition.error());
  }
  result << *condition;
  decreaseIndent();

  for (const auto &statement : node->body()->body()) {
    const auto val{statement->accept(*this)};
    if (!val) {
      return createError(val.error());
    }
    result << *val;
  }

  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_FOR *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_FOR");
  }
  std::ostringstream result;
  result << indent_ << "For Loop:\n";
  increaseIndent();

  result << indent_ << "Init:\n";
  increaseIndent();
  for (const auto &initExpr : node->init()) {
    const auto initStr{initExpr->accept(*this)};
    if (!initStr) {
      return createError(initStr.error());
    }
    result << *initStr;
  }
  decreaseIndent();

  const auto condition{node->condition()->accept(*this)};
  if (!condition) {
    return createError(condition.error());
  }
  result << *condition;

  result << indent_ << "Update:\n";
  increaseIndent();
  for (const auto &updateExpr : node->update()) {
    const auto updateStr{updateExpr->accept(*this)};
    if (!updateStr) {
      return createError(updateStr.error());
    }
    result << *updateStr;
  }
  decreaseIndent();

  for (const auto &statement : node->body()->body()) {
    const auto val{statement->accept(*this)};
    if (!val) {
      return createError(val.error());
    }
    result << *val;
  }

  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_DO_WHILE *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_DO_WHILE");
  }
  std::ostringstream result;
  result << indent_ << "Do While Loop:\n";
  increaseIndent();

  for (const auto &statement : node->body()->body()) {
    const auto val{statement->accept(*this)};
    if (!val) {
      return createError(val.error());
    }
    result << *val;
  }
  const auto condition{node->condition()->accept(*this)};
  if (!condition) {
    return createError(condition.error());
  }
  result << *condition;
  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_PASS *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_PASS");
  }
  std::ostringstream result;
  increaseIndent();
  result << indent_ << "Pass Statement:\n";
  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_STOP *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_STOP");
  }
  std::ostringstream result;
  increaseIndent();
  result << indent_ << "Stop Statement:\n";
  decreaseIndent();
  return result.str();
}

} // namespace nicole