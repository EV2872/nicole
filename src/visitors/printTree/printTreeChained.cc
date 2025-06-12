#include "../../../inc/parsingAnalysis/ast/chained/ast_chained.h"
#include "../../../inc/visitors/printTree/printTree.h"
#include <ostream>

namespace nicole {

auto PrintTree::visit(const AST_CHAINED *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_CHAINED");
  }
  std::ostringstream result;
  result << indent_ << "Chained:\n";
  increaseIndent();
  if (!node->base()) {
    return createError(ERROR_TYPE::NULL_NODE, "print base");
  }
  const auto base{node->base()->accept(*this)};
  if (!base) {
    return createError(ERROR_TYPE::NULL_NODE, "print base boo");
  }
  result << indent_ << "Base:\n" << *base;
  result << indent_ << "Operations:\n";
  for (const auto &statement : node->operations()) {
    const auto val{statement->accept(*this)};
    if (!val) {
      return createError(val.error());
    }
    result << *val;
  }
  decreaseIndent();
  return result.str();
}

} // namespace nicole