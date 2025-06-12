#include "../../../inc/parsingAnalysis/ast/vector/ast_index.h"
#include "../../../inc/parsingAnalysis/ast/vector/ast_vector.h"
#include "../../../inc/visitors/typeAnalysis/typeAnalysis.h"
#include <cstddef>
#include <memory>

namespace nicole {

/*
- si esta vacio retorna un vector<null>
- si no esta vacio, comprueba que todos los elementos sean de un mismo tipo
- si un elemento es un puntero userType debe comprobar que todos parten de una
misma base
*/
auto TypeAnalysis::visit(const AST_VECTOR *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_VECTOR");
  }
  const auto &values = node->values();
  if (values.empty()) {
    const auto emptyVecType{std::make_shared<VectorType>(typeTable_->null())};
    node->setReturnedFromAnalysis(emptyVecType);
    return emptyVecType;
  }
  auto firstResult = values[0]->accept(*this);
  if (!firstResult)
    return createError(firstResult.error());
  auto commonType = firstResult.value();
  /*
  if (insideDeclWithGenerics &&
      typeTable_->isGenericType(commonType, currentGenericList_))
    commonType = std::make_shared<PlaceHolder>(commonType);
  */
  for (size_t i = 1; i < values.size(); ++i) {
    auto result = values[i]->accept(*this);
    if (!result)
      return createError(result.error());
    auto elemType = result.value();

    if (auto constElem = std::dynamic_pointer_cast<ConstType>(elemType))
      elemType = constElem->baseType();
    /*
    if (insideDeclWithGenerics &&
        typeTable_->isGenericType(elemType, currentGenericList_))
      elemType = std::make_shared<PlaceHolder>(elemType);
    */
    if (!typeTable_->areSameType(commonType, elemType)) {
      if (!typeTable_->haveCommonAncestor(commonType, elemType))
        return createError(ERROR_TYPE::TYPE,
                           "vector elements do not have a common type");
    }
  }
  const auto vecType{std::make_shared<VectorType>(commonType)};
  node->setReturnedFromAnalysis(vecType);
  return vecType;
}

/*
- el value debe ser de tipo int
- se apoya en currentType para deducir el tipo del elemento al que accede
 */
auto TypeAnalysis::visit(const AST_INDEX *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_INDEX");
  }
  auto prevType = currentType_;
  const auto result{node->index()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }

  bool isGeneric{false};
  if (insideDeclWithGenerics and
      typeTable_->isGenericType(*result, currentGenericList_)) {
    isGeneric = true;
    // return *result;
  }

  if (!isGeneric) {
    if (!typeTable_->areSameType(*result, typeTable_->intType())) {
      return createError(ERROR_TYPE::TYPE, "index must be type int");
    }
  }

  const auto vectorType{std::dynamic_pointer_cast<VectorType>(prevType)};
  const auto basicType{std::dynamic_pointer_cast<BasicType>(prevType)};
  if (!vectorType and !basicType) {
    return createError(ERROR_TYPE::TYPE,
                       "can only access to vectors or strings");
  }
  std::shared_ptr<Type> indexType{nullptr};
  if (vectorType) {
    indexType = vectorType->elementType();
    node->setReturnedFromAnalysis(indexType);
  }
  if (basicType) {
    if (basicType->baseKind() != BasicKind::Str) {
      return createError(ERROR_TYPE::TYPE,
                         "can only access to vectors or strings");
    }
    indexType = typeTable_->charType();
    node->setReturnedFromAnalysis(indexType);
  }
  currentType_ = indexType;
  return indexType;
}

} // namespace nicole