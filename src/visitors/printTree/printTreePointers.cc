#include "../../../inc/parsingAnalysis/ast/pointer/ast_delete.h"
#include "../../../inc/parsingAnalysis/ast/pointer/ast_deref.h"
#include "../../../inc/parsingAnalysis/ast/pointer/ast_new.h"
#include "../../../inc/visitors/printTree/printTree.h"
#include <ostream>

namespace nicole {

auto PrintTree::visit(const AST_DELETE *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_DELETE");
  }
  std::ostringstream result;
  result << indent_ << "delete:\n";
  increaseIndent();
  const auto val{node->value()->accept(*this)};
  if (!val) {
    return createError(val.error());
  }
  result << *val;
  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_NEW *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_NEW");
  }
  std::ostringstream result;
  result << indent_ << "new:\n";
  increaseIndent();
  const auto val{node->value()->accept(*this)};
  if (!val) {
    return createError(val.error());
  }
  result << *val;
  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_DEREF *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_DEREF");
  }
  std::ostringstream result;
  result << indent_ << "deref:\n";
  increaseIndent();
  const auto val{node->value()->accept(*this)};
  if (!val) {
    return createError(val.error());
  }
  result << *val;
  decreaseIndent();
  return result.str();
}

} // namespace nicole