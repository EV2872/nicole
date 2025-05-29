#include "../../../inc/parsingAnalysis/ast/variables/ast_autoDecl.h"
#include "../../../inc/parsingAnalysis/ast/variables/ast_typedDecl.h"
#include "../../../inc/parsingAnalysis/ast/variables/ast_varCall.h"
#include "../../../inc/visitors/fillSemanticInfo/fillSemanticInfo.h"
#include <memory>
#include <variant>

namespace nicole {

std::expected<std::monostate, Error>
FillSemanticInfo::visit(const AST_AUTO_DECL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_AUTO_DECL");
  }

  const auto value{node->value()->accept(*this)};
  if (!value) {
    return createError(value.error());
  }

  if (analyzingInsideClass and currentUserType_->hasAttribute(node->id())) {
    return createError(ERROR_TYPE::ATTR, " the variable " + node->id() +
                                             " is shadowing the atribute");
  }

  const auto insert{currentScope_->insert(
      std::make_shared<Variable>(node->id(), nullptr, nullptr))};
  if (!insert) {
    return createError(insert.error());
  }

  return {};
}

std::expected<std::monostate, Error>
FillSemanticInfo::visit(const AST_VAR_TYPED_DECL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_VAR_TYPED_DECL");
  }

  const auto value{node->value()->accept(*this)};
  if (!value) {
    return createError(value.error());
  }

  if (analyzingInsideClass and currentUserType_->hasAttribute(node->id())) {
    return createError(ERROR_TYPE::ATTR, " the variable " + node->id() +
                                             " is shadowing the atribute");
  }

  auto varType{node->varType()};
  if (!typeTable_->isPossibleType(node->varType()) and
      !typeTable_->isGenericType(node->varType(), currentGenericList_)) {
    return createError(ERROR_TYPE::TYPE,
                       node->varType()->toString() +
                           " is not a posibble type or generic");
  }

  const auto checkIfMaskedEnum{typeTable_->isCompundEnumType(node->varType())};
  if (checkIfMaskedEnum) {
    varType = *checkIfMaskedEnum;
    node->setVarType(varType);
  }

  auto checkIfMaskedUserType{typeTable_->isCompundUserType(node->varType())};
  if (checkIfMaskedUserType) {
    varType = *checkIfMaskedUserType;
    node->setVarType(varType);
  }


  auto maybeGeneric =
      typeTable_->isCompundGenericType(varType, currentGenericList_);
  if (maybeGeneric.has_value()) {
    varType = *maybeGeneric;
    node->setVarType(varType);
  }

  const auto insert{currentScope_->insert(
      std::make_shared<Variable>(node->id(), varType, nullptr))};
  if (!insert) {
    return createError(insert.error());
  }

  return {};
}

std::expected<std::monostate, Error>
FillSemanticInfo::visit(const AST_VAR_CALL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_VAR_CALL");
  }
  if (analyzingInsideClass and currentUserType_->hasAttribute(node->id())) {
    return {};
  }
  if (!currentScope_->has(node->id())) {
    return createError(ERROR_TYPE::VARIABLE,
                       "the variable " + node->id() + " does not exist");
  }
  return {};
}

} // namespace nicole