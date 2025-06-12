#include "../../../inc/visitors/validateTree/validateTree.h"
#include "../../../inc/parsingAnalysis/ast/tree.h"

namespace nicole {

auto ValidateTree::visit(const AST_STATEMENT *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_STATEMENT");
  }
  return node->expression()->accept(*this);
}

auto ValidateTree::visit(const AST_BODY *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_BODY");
  }
  for (const auto &statement : node->body()) {
    const auto val{statement->accept(*this)};
    if (!val) {
      return createError(val.error());
    }
  }
  return true;
}

auto ValidateTree::visit(const Tree *tree) const noexcept
    -> std::expected<bool, Error> {
  if (!tree) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid Tree");
  }
  return tree->root()->accept(*this);
}

} // namespace nicole