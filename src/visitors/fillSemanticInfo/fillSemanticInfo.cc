#include "../../../inc/visitors/fillSemanticInfo/fillSemanticInfo.h"
#include "../../../inc/parsingAnalysis/ast/tree.h"
#include <cstddef>
#include <memory>
#include <variant>

/**

FillSemanticInfo ---> insertar delcaraciones en las tablas / insertar tipos /
comprobar que las variables pertenecen al scope (llamadas a variables) /
comrpobar llamadas a enum

TypeAnalysis ---> comprobar en una llamada a funcion que esta existe debido a
sobrecarga de funciones requiere que se trate en el typeAnalysis / igual con
metodos / llamadas a atributos / variables auto

 */

namespace nicole {

void FillSemanticInfo::pushScope() const noexcept {
  auto newScope = std::make_shared<Scope>(currentScope_);
  currentScope_ = newScope;
  if (!firstScope_) {
    firstScope_ = currentScope_;
  }
}

void FillSemanticInfo::popScope() const noexcept {
  if (currentScope_) {
    currentScope_ = currentScope_->father();
  }
}

std::expected<std::vector<GenericParameter>, Error>
FillSemanticInfo::mergeGenericLists(
    const std::vector<GenericParameter> &list,
    const std::vector<GenericParameter> &list1) const noexcept {
  if (hasDuplicatedGenerics(list)) {
    return createError(ERROR_TYPE::TYPE, "has duplicated generics");
  }
  if (hasDuplicatedGenerics(list1)) {
    return createError(ERROR_TYPE::TYPE, "has duplicated generics");
  }
  std::vector<GenericParameter> result{};
  result.insert(result.end(), list.begin(), list.end());
  result.insert(result.end(), list1.begin(), list1.end());
  if (hasDuplicatedGenerics(result)) {
    return createError(ERROR_TYPE::TYPE, "has duplicated generics");
  }
  return result;
}

bool FillSemanticInfo::hasDuplicatedGenerics(
    const std::vector<GenericParameter> &list) const noexcept {
  std::unordered_set<GenericParameter> set(list.begin(), list.end());
  return set.size() != list.size();
}

bool FillSemanticInfo::areAmbiguousFunctions(
    const Function &first, const Function &second) const noexcept {
  if (first.id() != second.id()) {
    return false;
  }
  if (first.generics().size() != second.generics().size()) {
    return false;
  }
  if (first.params().size() != second.params().size()) {
    return false;
  }
  const auto parameters = first.params().params();
  const auto parametersOther = second.params().params();
  for (size_t i = 0; i < parameters.size(); ++i) {
    if (!typeTable_->areSameType(parameters[i].second,
                                 parametersOther[i].second)) {
      if (!(typeTable_->isGenericType(parameters[i].second, first.generics()) &&
            typeTable_->isGenericType(parametersOther[i].second,
                                      second.generics()))) {
        return false;
      }
    }
  }
  return true;
}

bool FillSemanticInfo::areAmbiguousMethods(
    const Method &first, const Method &second) const noexcept {
  if (first.id() != second.id()) {
    return false;
  }
  if (first.generics().size() != second.generics().size()) {
    return false;
  }
  if (first.params().size() != second.params().size()) {
    return false;
  }
  const auto parameters = first.params().params();
  const auto parametersOther = second.params().params();
  for (size_t i = 0; i < parameters.size(); ++i) {
    if (!typeTable_->areSameType(parameters[i].second,
                                 parametersOther[i].second)) {
      if (!(typeTable_->isGenericType(parameters[i].second, first.generics()) &&
            typeTable_->isGenericType(parametersOther[i].second,
                                      second.generics()))) {
        return false;
      }
    }
  }
  return true;
}

std::expected<std::monostate, Error>
FillSemanticInfo::visit(const AST_STATEMENT *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_STATEMENT");
  }
  return node->expression()->accept(*this);
}

std::expected<std::monostate, Error>
FillSemanticInfo::visit(const AST_BODY *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_BODY");
  }
  for (const auto &expr : node->body()) {
    const auto result{expr->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return {};
}

std::expected<std::monostate, Error>
FillSemanticInfo::visit(const Tree *tree) const noexcept {
  if (!tree) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid Tree");
  }
  pushScope();
  tree->root()->setScope(currentScope_);
  const auto result{tree->root()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  popScope();
  if (validateMode_) {
    const auto mainFunction{functionTable_->getFunctions("main")};
    if (mainFunction.empty() or mainFunction.size() > 1) {
      return createError(ERROR_TYPE::FUNCTION,
                         "if the validation is activated the user must "
                         "specifiy a main function and only one instance");
    }
    if (mainFunction[0].returnType()->toString() !=
        BasicType{BasicKind::Int}.toString()) {
      return createError(ERROR_TYPE::FUNCTION,
                         "if the validation is activated the user must "
                         "specifiy a main function and only one instance of it "
                         "that returns type int");
    }
  }
  return {};
}

} // namespace nicole