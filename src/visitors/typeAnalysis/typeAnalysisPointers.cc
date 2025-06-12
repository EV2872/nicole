#include "../../../inc/parsingAnalysis/ast/pointer/ast_delete.h"
#include "../../../inc/parsingAnalysis/ast/pointer/ast_deref.h"
#include "../../../inc/parsingAnalysis/ast/pointer/ast_new.h"
#include "../../../inc/visitors/typeAnalysis/typeAnalysis.h"
#include <memory>

namespace nicole {

/*
- value debe ser un puntero y que este en la tabla de memoria dinamica
- retorna NoPropagate
 */
auto TypeAnalysis::visit(const AST_DELETE *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_DELETE");

  // Se analiza el valor que se desea borrar.
  auto result = node->value()->accept(*this);
  if (!result)
    return createError(result.error());

  auto type = result.value();

  if (insideDeclWithGenerics &&
      typeTable_->isGenericType(type, currentGenericList_)) {
    std::shared_ptr<Type> unwrapped = type;
    if (auto constType = std::dynamic_pointer_cast<ConstType>(unwrapped))
      unwrapped = constType->baseType();
    auto pointerType = std::dynamic_pointer_cast<PointerType>(unwrapped);
    if (!pointerType)
      return createError(ERROR_TYPE::TYPE,
                         "Delete on generic: type is not a pointer");
    if (!std::dynamic_pointer_cast<PlaceHolder>(pointerType->baseType()))
      return createError(
          ERROR_TYPE::TYPE,
          "Delete on generic: pointer does not wrap a PlaceHolder");

    node->setReturnedFromAnalysis(typeTable_->noPropagateType());
    return typeTable_->noPropagateType();
  }

  std::shared_ptr<PointerType> pointer =
      std::dynamic_pointer_cast<PointerType>(type);
  if (!pointer) {
    if (auto constType = std::dynamic_pointer_cast<ConstType>(type)) {
      pointer = std::dynamic_pointer_cast<PointerType>(constType->baseType());
      if (!pointer)
        return createError(ERROR_TYPE::TYPE, "can only delete pointers");
    } else {
      return createError(ERROR_TYPE::TYPE, "can only delete pointers");
    }
  }

  node->setReturnedFromAnalysis(typeTable_->noPropagateType());
  return typeTable_->noPropagateType();
}

/*
- envuelve en un puntero la expression y lo retorna, debe ser constructor o
primitiva
 */
auto TypeAnalysis::visit(const AST_NEW *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_NEW");
  }

  auto result = node->value()->accept(*this);
  if (!result)
    return createError(result.error());

  auto exprType = result.value();

  if (insideDeclWithGenerics and
      typeTable_->isGenericType(exprType, currentGenericList_))
    return std::make_shared<PointerType>(exprType);

  auto basicType = std::dynamic_pointer_cast<BasicType>(exprType);
  auto userType = std::dynamic_pointer_cast<UserType>(exprType);
  if (!basicType && !userType)
    return createError(
        ERROR_TYPE::TYPE,
        "can only use new with primitives, user types or generics");

  const auto wrapperTYpe{std::make_shared<PointerType>(exprType)};
  node->setReturnedFromAnalysis(wrapperTYpe);
  return wrapperTYpe;
}

/*
- la expresion debe ser un puntero y retorna el tipo al que apunta
*/
auto TypeAnalysis::visit(const AST_DEREF *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_DEREF");
  }

  auto result = node->value()->accept(*this);
  if (!result)
    return createError(result.error());

  auto type = result.value();

  std::shared_ptr<Type> unwrappedType = type;
  if (auto constType = std::dynamic_pointer_cast<ConstType>(type))
    unwrappedType = constType->baseType();

  if (insideDeclWithGenerics &&
      typeTable_->isGenericType(unwrappedType, currentGenericList_)) {

    if (auto ptrType = std::dynamic_pointer_cast<PointerType>(unwrappedType)) {
      node->setReturnedFromAnalysis(ptrType->baseType());
      return ptrType->baseType();
    }
    return createError(ERROR_TYPE::TYPE, "can only deref a pointer");
  }

  if (auto ptrType = std::dynamic_pointer_cast<PointerType>(unwrappedType)) {
    node->setReturnedFromAnalysis(ptrType->baseType());
    return ptrType->baseType();
  }
  return createError(ERROR_TYPE::TYPE, "can only deref a pointer");
}

} // namespace nicole