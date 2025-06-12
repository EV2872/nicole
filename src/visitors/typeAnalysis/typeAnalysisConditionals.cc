#include "../../../inc/parsingAnalysis/ast/conditionals/ast_case.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_default.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_elseIf.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_if.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_switch.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_ternary.h"
#include "../../../inc/visitors/typeAnalysis/typeAnalysis.h"
#include <cstddef>
#include <memory>

namespace nicole {

/*
- codition debe ser bool
- body lo mismo que el resto
- else if lo mismo que if
- else lo mismo que body
*/
auto TypeAnalysis::visit(const AST_IF *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_IF");
  }
  auto condition = node->condition()->accept(*this);
  if (!condition)
    return createError(condition.error());
  auto condType = condition.value();

  if (auto constCond = std::dynamic_pointer_cast<ConstType>(condType))
    condType = constCond->baseType();

  bool isGeneric{false};
  if (insideDeclWithGenerics &&
      typeTable_->isGenericType(condType, currentGenericList_)) {
    isGeneric = true;
    // return condType;
  }

  if (!isGeneric) {
    if (!typeTable_->areSameType(condType, typeTable_->boolType()))
      return createError(ERROR_TYPE::TYPE, "a condition must be boolean");
  }

  auto mainBody = node->body()->accept(*this);
  if (!mainBody)
    return createError(mainBody.error());

  std::vector<std::shared_ptr<Type>> branchTypes;

  if (!typeTable_->areSameType(mainBody.value(),
                               typeTable_->noPropagateType()) &&
      !typeTable_->areSameType(mainBody.value(), typeTable_->breakType())) {
    branchTypes.push_back(mainBody.value());
  }

  for (const auto &elseIf : node->elseIf()) {
    auto elseIfResult = elseIf->accept(*this);
    if (!elseIfResult)
      return createError(elseIfResult.error());
    if (!typeTable_->areSameType(elseIfResult.value(),
                                 typeTable_->noPropagateType()) &&
        !typeTable_->areSameType(elseIfResult.value(),
                                 typeTable_->breakType())) {
      branchTypes.push_back(elseIfResult.value());
    }
  }

  if (node->elseBody()) {
    auto elseBody = node->elseBody()->accept(*this);
    if (!elseBody)
      return createError(elseBody.error());
    if (!typeTable_->areSameType(elseBody.value(),
                                 typeTable_->noPropagateType()) &&
        !typeTable_->areSameType(elseBody.value(), typeTable_->breakType())) {
      branchTypes.push_back(elseBody.value());
    }
  }

  if (!branchTypes.empty()) {
    auto commonType = branchTypes.front();
    for (size_t i = 1; i < branchTypes.size(); ++i) {
      if (!typeTable_->areSameType(commonType, branchTypes[i])) {
        if (!typeTable_->haveCommonAncestor(commonType, branchTypes[i])) {
          return createError(ERROR_TYPE::TYPE,
                             "inconsistent return types in if branches");
        }
      }
    }
    node->setReturnedFromAnalysis(commonType);
    return commonType;
  }
  node->setReturnedFromAnalysis(typeTable_->noPropagateType());
  return typeTable_->noPropagateType();
}

/*
- lo mismo que if
*/
auto TypeAnalysis::visit(const AST_ELSE_IF *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ELSE_IF");
  }
  auto condition = node->condition()->accept(*this);
  if (!condition)
    return createError(condition.error());
  auto condType = condition.value();

  if (auto constCond = std::dynamic_pointer_cast<ConstType>(condType))
    condType = constCond->baseType();

  bool isGeneric{false};
  if (insideDeclWithGenerics &&
      typeTable_->isGenericType(condType, currentGenericList_)) {
    isGeneric = true;
    // return condType;
  }

  if (!isGeneric) {
    if (!typeTable_->areSameType(condType, typeTable_->boolType()))
      return createError(ERROR_TYPE::TYPE, "a condition must be boolean");
  }

  auto body = node->body()->accept(*this);
  if (!body)
    return createError(body.error());
  auto bodyType = body.value();

  if (!typeTable_->areSameType(bodyType, typeTable_->noPropagateType()) &&
      !typeTable_->areSameType(bodyType, typeTable_->breakType())) {
    node->setReturnedFromAnalysis(bodyType);
    return bodyType;
  }
  node->setReturnedFromAnalysis(typeTable_->noPropagateType());
  return typeTable_->noPropagateType();
}

/*
- match solo bool, int, char
- Lo mismo que if
*/
auto TypeAnalysis::visit(const AST_SWITCH *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_SWITCH");
  }
  auto condition = node->condition()->accept(*this);
  if (!condition)
    return createError(condition.error());
  auto condType = condition.value();
  if (auto constCond = std::dynamic_pointer_cast<ConstType>(condType))
    condType = constCond->baseType();

  bool isGeneric{false};
  if (insideDeclWithGenerics &&
      typeTable_->isGenericType(condType, currentGenericList_)) {
    isGeneric = true;
    // return condType;
  }

  if (!isGeneric) {
    bool isEnum = (std::dynamic_pointer_cast<EnumType>(condType) != nullptr);
    if (!(typeTable_->areSameType(condType, typeTable_->boolType()) ||
          typeTable_->areSameType(condType, typeTable_->intType()) ||
          typeTable_->areSameType(condType, typeTable_->charType()) ||
          isEnum)) {
      return createError(ERROR_TYPE::TYPE,
                         "switch condition must be bool, int, char, or enum");
    }
  }

  switchTypeCondition_ = condType;
  std::vector<std::shared_ptr<Type>> branchTypes;
  for (const auto &caseNode : node->cases()) {
    auto caseResult = caseNode->accept(*this);
    if (!caseResult)
      return createError(caseResult.error());
    auto caseType = caseResult.value();
    if (!typeTable_->areSameType(caseType, typeTable_->noPropagateType()) &&
        !typeTable_->areSameType(caseType, typeTable_->breakType())) {
      branchTypes.push_back(caseType);
    }
  }
  if (node->defaultCase()) {
    auto defaultResult = node->defaultCase()->accept(*this);
    if (!defaultResult)
      return createError(defaultResult.error());
    auto defaultType = defaultResult.value();
    if (!typeTable_->areSameType(defaultType, typeTable_->noPropagateType()) &&
        !typeTable_->areSameType(defaultType, typeTable_->breakType())) {
      branchTypes.push_back(defaultType);
    }
  }
  if (!branchTypes.empty()) {
    auto commonType = branchTypes.front();
    for (size_t i = 1; i < branchTypes.size(); ++i) {
      if (!typeTable_->areSameType(commonType, branchTypes[i])) {
        if (!typeTable_->haveCommonAncestor(commonType, branchTypes[i])) {
          return createError(ERROR_TYPE::TYPE,
                             "inconsistent return types in switch branches");
        }
      }
    }
    node->setReturnedFromAnalysis(commonType);
    return commonType;
  }
  node->setReturnedFromAnalysis(typeTable_->noPropagateType());
  return typeTable_->noPropagateType();
}

/*
- match solo bool, int char
- igual que body
*/
auto TypeAnalysis::visit(const AST_CASE *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_CASE");
  }
  auto matchResult = node->match()->accept(*this);
  if (!matchResult)
    return createError(matchResult.error());
  auto matchType = matchResult.value();

  if (auto constMatch = std::dynamic_pointer_cast<ConstType>(matchType))
    matchType = constMatch->baseType();

  bool isGeneric{false};
  if (insideDeclWithGenerics &&
      typeTable_->isGenericType(matchType, currentGenericList_)) {
    isGeneric = true;
    // return matchType;
  }

  if (!isGeneric) {
    if (!typeTable_->areSameType(matchType, switchTypeCondition_))
      return createError(
          ERROR_TYPE::TYPE,
          "case match type does not match switch condition type");
  }

  auto bodyResult = node->body()->accept(*this);
  if (!bodyResult)
    return createError(bodyResult.error());
  auto bodyType = bodyResult.value();

  if (!typeTable_->areSameType(bodyType, typeTable_->noPropagateType()) &&
      !typeTable_->areSameType(bodyType, typeTable_->breakType())) {
    node->setReturnedFromAnalysis(bodyType);
    return bodyType;
  }
  node->setReturnedFromAnalysis(typeTable_->noPropagateType());
  return typeTable_->noPropagateType();
}

/*
- igual que body
*/
auto TypeAnalysis::visit(const AST_DEFAULT *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_DEFAULT");
  }
  auto bodyResult = node->body()->accept(*this);
  if (!bodyResult)
    return createError(bodyResult.error());
  auto bodyType = bodyResult.value();

  if (!typeTable_->areSameType(bodyType, typeTable_->noPropagateType()) &&
      !typeTable_->areSameType(bodyType, typeTable_->breakType())) {
    node->setReturnedFromAnalysis(bodyType);
    return bodyType;
  }
  node->setReturnedFromAnalysis(typeTable_->noPropagateType());
  return typeTable_->noPropagateType();
}

/*
- condtion debe ser bool
- first y second no pueden ser breakType
 */
auto TypeAnalysis::visit(const AST_TERNARY *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_TERNARY");
  }
  auto condition = node->condition()->accept(*this);
  if (!condition)
    return createError(condition.error());
  auto condType = condition.value();

  if (auto constCond = std::dynamic_pointer_cast<ConstType>(condType))
    condType = constCond->baseType();

  bool isGeneric{false};
  if (insideDeclWithGenerics &&
      typeTable_->isGenericType(condType, currentGenericList_)) {
    isGeneric = true;
    // return condType;
  }

  if (!isGeneric) {
    if (!typeTable_->areSameType(condType, typeTable_->boolType()))
      return createError(ERROR_TYPE::TYPE, "a condition must be boolean");
  }

  auto firstResult = node->first()->accept(*this);
  if (!firstResult)
    return createError(firstResult.error());
  auto firstType = firstResult.value();
  /*
  if (insideDeclWithGenerics &&
      typeTable_->isGenericType(firstType, currentGenericList_))
    firstType = std::make_shared<PlaceHolder>(firstType);
  */
  auto secondResult = node->second()->accept(*this);
  if (!secondResult)
    return createError(secondResult.error());
  auto secondType = secondResult.value();
  /*
  if (insideDeclWithGenerics &&
      typeTable_->isGenericType(secondType, currentGenericList_))
    secondType = std::make_shared<PlaceHolder>(secondType);
  */
  if (typeTable_->areSameType(firstType, secondType)) {
    node->setReturnedFromAnalysis(firstType);
    return firstType;
  } else if (!typeTable_->haveCommonAncestor(firstType, secondType))
    return createError(ERROR_TYPE::TYPE,
                       "inconsistent types in ternary operator");
  else {
    node->setReturnedFromAnalysis(firstType);
    return firstType;
  }
}

/*
- debe retornar bool
*/
auto TypeAnalysis::visit(const AST_CONDITION *node) const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_CONDITION");
  }
  auto condResult = node->condition()->accept(*this);
  if (!condResult)
    return createError(condResult.error());
  auto condType = condResult.value();

  if (auto constCond = std::dynamic_pointer_cast<ConstType>(condType))
    condType = constCond->baseType();

  if (insideDeclWithGenerics &&
      typeTable_->isGenericType(condType, currentGenericList_)) {
    return condType;
  }

  bool isEnum = (std::dynamic_pointer_cast<EnumType>(condType) != nullptr);

  if (!std::dynamic_pointer_cast<PointerType>(condType) &&
      !typeTable_->areSameType(condType, typeTable_->boolType()) &&
      !typeTable_->areSameType(condType, typeTable_->intType()) &&
      !typeTable_->areSameType(condType, typeTable_->charType()) && !isEnum) {
    return createError(
        ERROR_TYPE::TYPE,
        "condition must be a ptr, bool, int, char, or enum(in a switch)");
  }
  node->setReturnedFromAnalysis(condType);
  return condType;
}

} // namespace nicole