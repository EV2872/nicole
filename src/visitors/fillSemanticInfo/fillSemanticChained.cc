#include "../../../inc/parsingAnalysis/ast/chained/ast_chained.h"
#include "../../../inc/visitors/fillSemanticInfo/fillSemanticInfo.h"
#include <variant>

namespace nicole {

auto FillSemanticInfo::visit(const AST_CHAINED *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_CHAINED");
  }
  const auto base{node->base()->accept(*this)};
  if (!base) {
    return createError(base.error());
  }
  for (const auto &operations : node->operations()) {
    const auto result{operations->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return {};
}

} // namespace nicole