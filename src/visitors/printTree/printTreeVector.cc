#include "../../../inc/visitors/printTree/printTree.h"
#include "../../../inc/parsingAnalysis/ast/vector/ast_index.h"
#include "../../../inc/parsingAnalysis/ast/vector/ast_vector.h"
#include <ostream>

namespace nicole {

std::expected<std::string, Error>
PrintTree::visit(const AST_VECTOR *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_VECTOR");
  }
  std::ostringstream result;
  result << indent_ << "vector:\n";
  increaseIndent();
  for (const auto &statement : node->values()) {
    const auto val{statement->accept(*this)};
    if (!val) {
      return createError(val.error());
    }
    result << *val;
  }
  decreaseIndent();
  return result.str();
}

std::expected<std::string, Error>
PrintTree::visit(const AST_INDEX *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_INDEX");
  }
  std::ostringstream result;
  result << indent_ << "index:\n";
  increaseIndent();
  const auto val{node->index()->accept(*this)};
  if (!val) {
    return createError(val.error());
  }
  result << *val;
  decreaseIndent();
  return result.str();
}

}