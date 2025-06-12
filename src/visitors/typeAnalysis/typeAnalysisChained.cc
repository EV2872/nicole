#include "../../../inc/parsingAnalysis/ast/chained/ast_chained.h"
#include "../../../inc/visitors/typeAnalysis/typeAnalysis.h"
#include <memory>

namespace nicole {

/*
- comprueba que base no sea void
- retorna ultimo tipo encontrado
*/
auto TypeAnalysis::visit(const AST_CHAINED *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_CHAINED");
  }
  auto baseRes = node->base()->accept(*this);
  if (!baseRes)
    return createError(baseRes.error());
  if (const auto enumType{std::dynamic_pointer_cast<EnumType>(*baseRes)}) {
    if (node->operations().size()) {
      return createError(
          ERROR_TYPE::TYPE,
          "an enum constructor call cannot appear in a chained expression");
    }
    node->setReturnedFromAnalysis(*baseRes);
    return *baseRes;
  }
  if (const auto placeHolder{
          std::dynamic_pointer_cast<PlaceHolder>(*baseRes)}) {
    node->setReturnedFromAnalysis(*baseRes);
    return *baseRes;
  }
  currentType_ = baseRes.value();

  for (const auto &op : node->operations()) {
    auto opRes = op->accept(*this);
    if (!opRes)
      return createError(opRes.error());
  }
  node->setReturnedFromAnalysis(currentType_);
  return currentType_;
}

} // namespace nicole