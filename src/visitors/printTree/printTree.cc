#include "../../../inc/visitors/printTree/printTree.h"
#include "../../../inc/parsingAnalysis/ast/tree.h"
#include <ostream>

namespace nicole {

auto PrintTree::visit(const AST_STATEMENT *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_STATEMENT");
  }
  std::ostringstream result;
  result << indent_ << "statement:\n";
  increaseIndent();
  const auto expr{node->expression()->accept(*this)};
  if (!expr) {
    return createError(expr.error());
  }
  result << *expr;
  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_BODY *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_BODY");
  }
  std::ostringstream result;
  result << indent_ << "Body:\n";
  increaseIndent();
  for (const auto &statement : node->body()) {
    const auto val{statement->accept(*this)};
    if (!val) {
      return createError(val.error());
    }
    result << *val;
  }
  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const Tree *tree) const noexcept
    -> std::expected<std::string, Error> {
  if (!tree) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid Tree");
  }
  std::ostringstream result;
  result << "Tree:\n";
  increaseIndent();
  const auto str{tree->root()->accept(*this)};
  if (!str) {
    return createError(str.error());
  }
  result << *str;
  decreaseIndent();
  return result.str();
}

} // namespace nicole