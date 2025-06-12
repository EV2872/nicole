#include "../../../inc/parsingAnalysis/ast/vector/ast_index.h"
#include "../../../inc/parsingAnalysis/ast/vector/ast_vector.h"
#include "../../../inc/visitors/monomorphize/monomorphize.h"
#include <variant>

namespace nicole {

auto Monomorphize::visit(const AST_VECTOR *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_VECTOR");
  }
  for (const auto &value : node->values()) {
    const auto result{value->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return {};
}

auto Monomorphize::visit(const AST_INDEX *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_INDEX");
  }
  const auto result{node->index()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return {};
}

} // namespace nicole