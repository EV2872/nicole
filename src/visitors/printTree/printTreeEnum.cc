#include "../../../inc/parsingAnalysis/ast/enum/ast_enum.h"
#include "../../../inc/parsingAnalysis/ast/enum/ast_enumAccess.h"
#include "../../../inc/visitors/printTree/printTree.h"
#include <ostream>

namespace nicole {

auto PrintTree::visit(const AST_ENUM *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ENUM");
  }
  std::ostringstream result;
  result << indent_ << "Enum:\n";
  increaseIndent();
  result << indent_ << "id: " << node->id() << "\n";
  result << indent_ << "names:\n";
  for (const auto &id : node->identifiers()) {
    result << indent_ << id << "\n";
  }
  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_ENUM_ACCESS *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ENUM_ACCESS");
  }
  std::ostringstream result;
  result << indent_ << "Enum Access:\n";
  increaseIndent();
  result << indent_ << "id: " << node->id() << "\n";
  result << indent_ << "name: " + node->identifier() + "\n";
  decreaseIndent();
  return result.str();
}

} // namespace nicole