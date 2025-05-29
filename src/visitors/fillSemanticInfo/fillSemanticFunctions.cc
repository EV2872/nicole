#include "../../../inc/parsingAnalysis/ast/functions/ast_funcCall.h"
#include "../../../inc/parsingAnalysis/ast/functions/ast_funcDecl.h"
#include "../../../inc/parsingAnalysis/ast/functions/ast_return.h"
#include "../../../inc/visitors/fillSemanticInfo/fillSemanticInfo.h"
#include <memory>
#include <variant>

namespace nicole {

std::expected<std::monostate, Error>
FillSemanticInfo::visit(const AST_FUNC_CALL *node) const noexcept {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_FUNC_CALL");

  if (functionTable_->getFunctions(node->id()).empty()) {
    return createError(ERROR_TYPE::FUNCTION,
                       "no function with id: " + node->id() + " exists");
  }

  for (std::size_t i = 0; i < node->replaceOfGenerics().size(); ++i) {
    auto replacement = node->replaceOfGenerics()[i];
    if (!typeTable_->isPossibleType(replacement) &&
        !typeTable_->isGenericType(replacement, currentGenericList_)) {
      return createError(ERROR_TYPE::TYPE,
                         replacement->toString() +
                             " is not a possible type or generic");
    }

    if (auto maskedEnum = typeTable_->isCompundEnumType(replacement)) {
      replacement = *maskedEnum;
      auto setRes = node->setGenericReplacement(i, replacement);
      if (!setRes)
        return createError(setRes.error());
    }

    if (auto maskedUserType = typeTable_->isCompundUserType(replacement)) {
      replacement = *maskedUserType;
      auto setRes = node->setGenericReplacement(i, replacement);
      if (!setRes)
        return createError(setRes.error());
    }

    if (auto maskedGeneric = typeTable_->isCompundGenericType(
            replacement, currentGenericList_)) {
      replacement = *maskedGeneric;
      auto setRes = node->setGenericReplacement(i, replacement);
      if (!setRes)
        return createError(setRes.error());
    }
  }

  for (const auto &expr : node->parameters()) {
    const auto result = expr->accept(*this);
    if (!result)
      return createError(result.error());
  }
  return {};
}

std::expected<std::monostate, Error>
FillSemanticInfo::visit(const AST_FUNC_DECL *node) const noexcept {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_FUNC_DECL");
  const Function newFunction{node->id(), node->generics(), node->parameters(),
                             node->returnType(), node->body()};

  const auto functions = functionTable_->getFunctions(newFunction.id());
  for (const auto &func : functions) {
    if (areAmbiguousFunctions(newFunction, func)) {
      return createError(ERROR_TYPE::FUNCTION,
                         "redeclaration of function: " + newFunction.id());
    }
  }
  newFunction.setDeclReference(node);
  functionTable_->insert(newFunction);

  pushScope();
  node->body()->setScope(currentScope_);

  currentGenericList_ = node->generics();
  if (hasDuplicatedGenerics(currentGenericList_)) {
    return createError(ERROR_TYPE::FUNCTION, "has duplicated generics");
  }
  node->setGenerics(currentGenericList_);

  std::vector<std::pair<std::string, std::shared_ptr<nicole::Type>>>
      newParameters;
  for (const auto &param : node->parameters()) {
    if (!typeTable_->isPossibleType(param.second) &&
        !typeTable_->isGenericType(param.second, currentGenericList_))
      return createError(ERROR_TYPE::TYPE,
                         param.second->toString() +
                             " is not a possible type or generic");

    auto newType = param.second;
    if (auto maskedEnum = typeTable_->isCompundEnumType(newType))
      newType = *maskedEnum;
    if (auto maskedUserType = typeTable_->isCompundUserType(newType))
      newType = *maskedUserType;
    if (auto maskedGeneric =
            typeTable_->isCompundGenericType(newType, currentGenericList_))
      newType = *maskedGeneric;

    newParameters.push_back({param.first, newType});
    if (auto insertResult = currentScope_->insert(
            std::make_shared<Variable>(param.first, newType, nullptr));
        !insertResult)
      return createError(insertResult.error());
  }
  Parameters params{newParameters};
  node->setParameters(params);

  if (auto setted = functionTable_->setFuncParameters(node->id(), params);
      !setted)
    return createError(setted.error());

  if (!typeTable_->isPossibleType(node->returnType()) &&
      !typeTable_->isGenericType(node->returnType(), currentGenericList_))
    return createError(ERROR_TYPE::TYPE,
                       node->returnType()->toString() +
                           " is not a possible type or generic");

  auto retType = node->returnType();
  if (auto maskedEnum = typeTable_->isCompundEnumType(retType))
    retType = *maskedEnum;
  if (auto maskedUserType = typeTable_->isCompundUserType(retType))
    retType = *maskedUserType;
  if (auto maskedGeneric =
          typeTable_->isCompundGenericType(retType, currentGenericList_))
    retType = *maskedGeneric;
  node->setReturnType(retType);

  if (auto settedReturnType =
          functionTable_->setFuncReturnType(node->id(), retType);
      !settedReturnType)
    return createError(settedReturnType.error());

  if (auto bodyResult = node->body()->accept(*this); !bodyResult)
    return createError(bodyResult.error());

  popScope();
  currentGenericList_.clear();
  return {};
}

std::expected<std::monostate, Error>
FillSemanticInfo::visit(const AST_RETURN *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_RETURN");
  }
  if (!node->expression()) {
    return {};
  }
  return node->expression()->accept(*this);
}

} // namespace nicole