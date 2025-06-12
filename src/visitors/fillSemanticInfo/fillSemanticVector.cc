#include "../../../inc/parsingAnalysis/ast/vector/ast_index.h"
#include "../../../inc/parsingAnalysis/ast/vector/ast_vector.h"
#include "../../../inc/visitors/fillSemanticInfo/fillSemanticInfo.h"
#include <variant>

namespace nicole {

auto FillSemanticInfo::visit(const AST_VECTOR *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_VECTOR");
  }
  for (const auto &expr : node->values()) {
    const auto result{expr->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return {};
}

auto FillSemanticInfo::visit(const AST_INDEX *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_INDEX");
  }
  return node->index()->accept(*this);
}

} // namespace nicole