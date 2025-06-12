#include "../../../inc/parsingAnalysis/ast/assignments/ast_assignment.h"
#include "../../../inc/visitors/monomorphize/monomorphize.h"
#include <variant>

namespace nicole {

auto Monomorphize::visit(const AST_ASSIGNMENT *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ASSIGNMENT");
  }
  const auto left{node->left()->accept(*this)};
  if (!left) {
    return createError(left.error());
  }
  const auto right{node->value()->accept(*this)};
  if (!right) {
    return createError(right.error());
  }
  return {};
}

} // namespace nicole