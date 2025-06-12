#include "../../../inc/parsingAnalysis/ast/assignments/ast_assignment.h"
#include "../../../inc/visitors/printTree/printTree.h"
#include <ostream>

namespace nicole {

auto PrintTree::visit(const AST_ASSIGNMENT *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ASSIGNMENT");
  }
  std::ostringstream result;
  result << indent_ << "Assignment:\n";
  increaseIndent();
  result << indent_ << "Operator: " << node->op().raw() << "\n";
  const auto left{node->left()->accept(*this)};
  if (!left) {
    return createError(left.error());
  }
  result << indent_ << "left:\n" << *left << "\n";
  const auto val{node->value()->accept(*this)};
  if (!val) {
    return createError(val.error());
  }
  result << indent_ << "Value:\n" << *val << "\n";
  decreaseIndent();
  return result.str();
}

} // namespace nicole