#include "../../../inc/visitors/monomorphize/monomorphize.h"
#include "../../../inc/parsingAnalysis/ast/operators/ast_binary.h"
#include "../../../inc/parsingAnalysis/ast/operators/ast_unary.h"
#include <variant>

namespace nicole {

std::expected<std::monostate, Error>
Monomorphize::visit(const AST_BINARY *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_BINARY");
  }
  const auto left{node->left()->accept(*this)};
  if (!left) {
    return createError(left.error());
  }
  const auto right{node->right()->accept(*this)};
  if (!right) {
    return createError(right.error());
  }
  return {};
}

std::expected<std::monostate, Error>
Monomorphize::visit(const AST_UNARY *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_UNARY");
  }
  const auto left{node->value()->accept(*this)};
  if (!left) {
    return createError(left.error());
  }
  return {};
}

}