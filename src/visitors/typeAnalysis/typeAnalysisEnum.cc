#include "../../../inc/parsingAnalysis/ast/enum/ast_enum.h"
#include "../../../inc/parsingAnalysis/ast/enum/ast_enumAccess.h"
#include "../../../inc/visitors/typeAnalysis/typeAnalysis.h"
#include <memory>

namespace nicole {

/*
- retorna NoPropagate
*/
auto TypeAnalysis::visit(const AST_ENUM *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ENUM");
  }
  const auto type{typeTable_->noPropagateType()};
  node->setReturnedFromAnalysis(type);
  return type;
}

auto TypeAnalysis::visit(const AST_ENUM_ACCESS *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ENUM_ACCESS");
  }
  const auto type{typeTable_->getType(node->id())};
  if (!type) {
    return createError(ERROR_TYPE::TYPE, "enum does not exist");
  }
  node->setReturnedFromAnalysis(*type);
  return type;
}

} // namespace nicole