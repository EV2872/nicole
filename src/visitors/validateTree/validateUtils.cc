#include "../../../inc/parsingAnalysis/ast/utils/ast_import.h"
#include "../../../inc/parsingAnalysis/ast/utils/ast_print.h"
#include "../../../inc/parsingAnalysis/checkPosition.h"
#include "../../../inc/visitors/validateTree/validateTree.h"

namespace nicole {

// statement / body / not null
auto ValidateTree::visit(const AST_PRINT *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_PRINT");
  }
  if (!CheckPosition::itsBodyAncestorHasParent(node)) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a print statement only can appear in a scope");
  }
  for (const auto &chain : node->values()) {
    const auto result{chain->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return true;
}

// statement / body / null
auto ValidateTree::visit(const AST_IMPORT *node) const noexcept
    -> std::expected<bool, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_IMPORT");
  }
  if (CheckPosition::itsBodyAncestorHasParent(node)) {
    return createError(ERROR_TYPE::VALIDATE_TREE,
                       "a import must appear outside of any scope");
  }
  return true;
}

} // namespace nicole