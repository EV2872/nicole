#include "../../../inc/parsingAnalysis/ast/literals/ast_bool.h"
#include "../../../inc/parsingAnalysis/ast/literals/ast_char.h"
#include "../../../inc/parsingAnalysis/ast/literals/ast_double.h"
#include "../../../inc/parsingAnalysis/ast/literals/ast_float.h"
#include "../../../inc/parsingAnalysis/ast/literals/ast_int.h"
#include "../../../inc/parsingAnalysis/ast/literals/ast_null.h"
#include "../../../inc/parsingAnalysis/ast/literals/ast_string.h"
#include "../../../inc/parsingAnalysis/checkPosition.h"
#include "../../../inc/visitors/validateTree/validateTree.h"

namespace nicole {

auto ValidateTree::visit(const AST_BOOL *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_BOOL");
  }
  if (CheckPosition::hasEveryAncestorInOrder(
          node, {AST_TYPE::STATEMENT, AST_TYPE::BODY})) {
    return createError(ERROR_TYPE::VALIDATE_TREE, "dangling bool");
  }
  return true;
}

auto ValidateTree::visit(const AST_CHAR *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_CHAR");
  }
  if (CheckPosition::hasEveryAncestorInOrder(
          node, {AST_TYPE::STATEMENT, AST_TYPE::BODY})) {
    return createError(ERROR_TYPE::VALIDATE_TREE, "dangling char");
  }
  return true;
}

auto ValidateTree::visit(const AST_DOUBLE *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_DOUBLE");
  }
  if (CheckPosition::hasEveryAncestorInOrder(
          node, {AST_TYPE::STATEMENT, AST_TYPE::BODY})) {
    return createError(ERROR_TYPE::VALIDATE_TREE, "dangling double");
  }
  return true;
}

auto ValidateTree::visit(const AST_FLOAT *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_FLOAT");
  }
  if (CheckPosition::hasEveryAncestorInOrder(
          node, {AST_TYPE::STATEMENT, AST_TYPE::BODY})) {
    return createError(ERROR_TYPE::VALIDATE_TREE, "dangling float");
  }
  return true;
}

auto ValidateTree::visit(const AST_INT *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_INT");
  }
  if (CheckPosition::hasEveryAncestorInOrder(
          node, {AST_TYPE::STATEMENT, AST_TYPE::BODY})) {
    return createError(ERROR_TYPE::VALIDATE_TREE, "dangling int");
  }
  return true;
}

auto ValidateTree::visit(const AST_NULL *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_NULL");
  }
  if (CheckPosition::hasEveryAncestorInOrder(
          node, {AST_TYPE::STATEMENT, AST_TYPE::BODY})) {
    return createError(ERROR_TYPE::VALIDATE_TREE, "dangling null");
  }
  return true;
}

auto ValidateTree::visit(const AST_STRING *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_STRING");
  }
  if (CheckPosition::hasEveryAncestorInOrder(
          node, {AST_TYPE::STATEMENT, AST_TYPE::BODY})) {
    return createError(ERROR_TYPE::VALIDATE_TREE, "dangling string");
  }
  return true;
}

} // namespace nicole