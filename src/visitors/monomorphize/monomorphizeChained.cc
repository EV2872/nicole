#include "../../../inc/parsingAnalysis/ast/chained/ast_chained.h"
#include "../../../inc/visitors/monomorphize/monomorphize.h"
#include <variant>

namespace nicole {

auto Monomorphize::visit(const AST_CHAINED *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_CHAINED");
  }
  const auto base{node->base()->accept(*this)};
  if (!base) {
    return createError(base.error());
  }
  for (const auto &chain : node->operations()) {
    const auto result{chain->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return {};
}

} // namespace nicole