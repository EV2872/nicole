#include "../../../inc/parsingAnalysis/ast/utils/ast_import.h"
#include "../../../inc/parsingAnalysis/ast/utils/ast_print.h"
#include "../../../inc/visitors/printTree/printTree.h"
#include <ostream>

namespace nicole {

auto PrintTree::visit(const AST_PRINT *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_PRINT");
  }
  std::ostringstream result;
  result << indent_ << "Print Statement:\n";
  increaseIndent();
  increaseIndent();
  for (const auto &expr : node->values()) {
    const auto toStr{expr->accept(*this)};
    if (!toStr) {
      return createError(toStr.error());
    }
    result << indent_ << "Expression:\n" << *toStr;
  }
  decreaseIndent();
  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_IMPORT *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_IMPORT");
  }
  std::ostringstream result;
  result << indent_ << "Import:\n";
  increaseIndent();
  result << indent_ << "File: " << node->path().string() << "\n";
  decreaseIndent();
  return result.str();
}

} // namespace nicole