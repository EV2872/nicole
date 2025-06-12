#include "../../../inc/parsingAnalysis/ast/chained/ast_chained.h"
#include "../../../inc/parsingAnalysis/checkPosition.h"
#include "../../../inc/visitors/validateTree/validateTree.h"

namespace nicole {

auto ValidateTree::visit(const AST_CHAINED *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_CHAINED");
  }
  if (CheckPosition::isOutOfScope(node)) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "chained expressions cannot appear outside of scopes");
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
  return true;
}

} // namespace nicole