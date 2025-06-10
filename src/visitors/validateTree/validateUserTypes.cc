#include "../../../inc/visitors/validateTree/validateTree.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_attrAccess.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_constructorCall.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_methodCall.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_struct.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_this.h"
#include "../../../inc/parsingAnalysis/checkPosition.h"

namespace nicole {

// statement / body / null
std::expected<bool, Error>
ValidateTree::visit(const AST_STRUCT *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_STRUCT");
  }
  if (CheckPosition::itsBodyAncestorHasParent(node)) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a struct declaration must be outside of any scope");
  }
  const auto constructor{node->constructor()->accept(*this)};
  if (!constructor) {
    return createError(constructor.error());
  }
  const auto destructor{node->destructor()->accept(*this)};
  if (!destructor) {
    return createError(destructor.error());
  }
  for (const auto &chain : node->methods()) {
    const auto result{chain->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return true;
}

// chained childs
std::expected<bool, Error>
ValidateTree::visit(const AST_ATTR_ACCESS *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ATTR_ACCESS");
  }
  if (!CheckPosition::hasAnyAncestorOf(
          node, {AST_TYPE::ATTR_ACCESS, AST_TYPE::METHOD_CALL, AST_TYPE::INDEX,
                 AST_TYPE::CONSTRUCTOR_CALL, AST_TYPE::VAR_CALL})) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "invalid hierarchy for attr access");
  }
  return true;
}

// chained childs
std::expected<bool, Error>
ValidateTree::visit(const AST_METHOD_CALL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_METHOD_CALL");
  }
  if (!CheckPosition::hasAnyAncestorOf(
          node, {AST_TYPE::ATTR_ACCESS, AST_TYPE::METHOD_CALL, AST_TYPE::INDEX,
                 AST_TYPE::CONSTRUCTOR_CALL, AST_TYPE::VAR_CALL})) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "invalid hierarchy for Method call");
  }
  for (const auto &chain : node->parameters()) {
    const auto result{chain->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return true;
}

std::expected<bool, Error>
ValidateTree::visit(const AST_METHOD_DECL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_METHOD_DECL");
  }
  if (!CheckPosition::hasEveryAncestorInOrder(node, {AST_TYPE::STRUCT_DECL})) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "invalid hierarchy for Method decl");
  }
  const auto result{node->body()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return true;
}

std::expected<bool, Error>
ValidateTree::visit(const AST_CONSTRUCTOR_DECL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_CONSTRUCTOR_DECL");
  }
  if (!CheckPosition::hasEveryAncestorInOrder(node, {AST_TYPE::STRUCT_DECL})) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "invalid hierarchy for constructor decl");
  }
  if (node->super()) {
    const auto result{node->super()->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  const auto result{node->body()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return true;
}

std::expected<bool, Error>
ValidateTree::visit(const AST_SUPER *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_SUPER");
  }
  if (!CheckPosition::hasEveryAncestorInOrder(node,
                                              {AST_TYPE::CONSTRUCTOR_DECL})) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "invalid hierarchy for destructor decl");
  }
  for (const auto &chain : node->arguments()) {
    const auto result{chain->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return true;
}

std::expected<bool, Error>
ValidateTree::visit(const AST_DESTRUCTOR_DECL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_DESTRUCTOR_DECL");
  }
  if (!CheckPosition::hasEveryAncestorInOrder(node, {AST_TYPE::STRUCT_DECL})) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "invalid hierarchy for destructor decl");
  }
  const auto result{node->body()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return true;
}

// func decl / struct
std::expected<bool, Error>
ValidateTree::visit(const AST_THIS *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_THIS");
  }
  if (!CheckPosition::hasAnyAncestorOf(node, {AST_TYPE::STRUCT_DECL})) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a this call can only appear in methods");
  }
  return true;
}

// chained
std::expected<bool, Error>
ValidateTree::visit(const AST_CONSTRUCTOR_CALL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_CONSTRUCTOR_CALL");
  }
  if (node->father()->type() != AST_TYPE::CHAIN) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "invalid hierachy AST_VAR_CALL");
  }
  for (const auto &chain : node->parameters()) {
    const auto result{chain->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return true;
}

}