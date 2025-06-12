#include "../../../inc/visitors/typeAnalysis/typeAnalysis.h"
#include "../../../inc/parsingAnalysis/ast/tree.h"
#include <cstddef>
#include <memory>

/**

FillSemanticInfo ---> insertar delcaraciones en las tablas / insertar tipos /
comprobar que las variables pertenecen al scope (llamadas a variables) /
comrpobar llamadas a enum

TypeAnalysis ---> comprobar en una llamada a funcion que esta existe debido a
sobrecarga de funciones requiere que se trate en el typeAnalysis / igual con
metodos / llamadas a atributos / variables auto

 */

namespace nicole {

auto TypeAnalysis::mergeGenericLists(const std::vector<GenericParameter> &list,
                                     const std::vector<GenericParameter> &list1)
    const noexcept -> std::vector<GenericParameter> {
  std::vector<GenericParameter> result{};
  result.insert(result.end(), list.begin(), list.end());
  result.insert(result.end(), list1.begin(), list1.end());
  return result;
}

/*
- retorna el tipo de la expresion
*/
auto TypeAnalysis::visit(const AST_STATEMENT *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_STATEMENT");
  }
  const auto result{node->expression()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  node->setReturnedFromAnalysis(*result);
  return result;
}

/*
- si encuentra pass y stop devuelve void
- si encuentra un return retorna su tipo, vijilar varios returns en diferentes
body anidados
- si no encuentra nada retorna NoPropagate
*/
auto TypeAnalysis::visit(const AST_BODY *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_BODY");
  }
  auto oldScope = currentScope_;
  currentScope_ = node->scope();
  std::vector<std::shared_ptr<Type>> returnTypes;
  bool foundBreak = false;

  for (const auto &expr : node->body()) {
    auto result = expr->accept(*this);
    if (!result)
      return createError(result.error());
    auto type = result.value();
    if (typeTable_->areSameType(type, typeTable_->noPropagateType()))
      continue;
    if (typeTable_->areSameType(type, typeTable_->breakType())) {
      foundBreak = true;
      continue;
    }
    if (firstBody) {
      if (!foundReturn) {
        continue;
      }
      foundReturn = false;
      if (!typeTable_->areSameType(type, typeTable_->voidType()) and
          !typeTable_->areSameType(type, typeTable_->noPropagateType()) and
          !typeTable_->areSameType(type, typeTable_->intType())) {
        return createError(
            ERROR_TYPE::TYPE,
            " if a return affects the root body it must be void/int and cannot "
            "be affected by both at the same time");
      }
      returnTypes.push_back(type);
    }
    if (!firstBody) {
      returnTypes.push_back(type);
    }
  }

  currentScope_ = oldScope;

  if (!returnTypes.empty() && foundBreak)
    return createError(ERROR_TYPE::TYPE,
                       "inconsistent return types: cannot mix return and "
                       "break/continue in the same body");
  if (!returnTypes.empty()) {
    auto commonType = returnTypes.front();
    for (size_t i = 1; i < returnTypes.size(); ++i) {
      if (!typeTable_->areSameType(commonType, returnTypes[i])) {
        if (!typeTable_->haveCommonAncestor(commonType, returnTypes[i])) {
          return createError(ERROR_TYPE::TYPE,
                             "inconsistent return types in body with id: " +
                                 std::to_string(node->nodeId()));
        }
      }
    }
    node->setReturnedFromAnalysis(commonType);
    return commonType;
  }
  if (foundBreak) {
    node->setReturnedFromAnalysis(typeTable_->breakType());
    return typeTable_->breakType();
  }
  node->setReturnedFromAnalysis(typeTable_->noPropagateType());
  return typeTable_->noPropagateType();
}

/*
- comprueba que no hubo errores
- retorna int / void
*/
auto TypeAnalysis::visit(const Tree *tree) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!tree) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid Tree");
  }
  auto result = tree->root()->accept(*this);
  if (!result)
    return createError(result.error());

  auto bodyType = result.value();
  if (auto constBody = std::dynamic_pointer_cast<ConstType>(bodyType))
    bodyType = constBody->baseType();

  if (!(typeTable_->areSameType(bodyType, typeTable_->intType()) ||
        typeTable_->areSameType(bodyType, typeTable_->voidType()) ||
        typeTable_->areSameType(bodyType, typeTable_->noPropagateType())))
    return createError(ERROR_TYPE::TYPE,
                       "body must return int, void, or noPropagate, got " +
                           bodyType->toString());
  secondAnalysis = true;

  return result;
}

} // namespace nicole