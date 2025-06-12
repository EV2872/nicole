#include "../../../inc/parsingAnalysis/ast/variables/ast_autoDecl.h"
#include "../../../inc/parsingAnalysis/ast/variables/ast_typedDecl.h"
#include "../../../inc/parsingAnalysis/ast/variables/ast_varCall.h"
#include "../../../inc/visitors/printTree/printTree.h"
#include <ostream>

namespace nicole {

auto PrintTree::visit(const AST_AUTO_DECL *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_AUTO_DECL");
  }
  std::ostringstream result;
  result << indent_ << "auto decl\n";
  increaseIndent();
  result << indent_ << "id: " << node->id() << "\n";
  result << indent_ << "const: " << ((node->isConst()) ? "true" : "false")
         << "\n";
  result << indent_ << "value:\n";
  const auto val{node->value()->accept(*this)};
  if (!val) {
    return createError(val.error());
  }
  result << *val << "\n";
  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_VAR_TYPED_DECL *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_VAR_TYPED_DECL");
  }
  std::ostringstream result;
  result << indent_ << "var typed decl\n";
  increaseIndent();
  result << indent_ << "id: " << node->id() << "\n";
  result << indent_ << "type: " << node->varType()->toString() << "\n";
  result << indent_ << "value:\n";
  const auto val{node->value()->accept(*this)};
  if (!val) {
    return createError(val.error());
  }
  result << *val << "\n";
  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_VAR_CALL *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_VAR_CALL");
  }
  std::ostringstream result;
  result << indent_ << "Var call\n";
  increaseIndent();
  result << indent_ << "id: " << node->id() << "\n";
  decreaseIndent();
  return result.str();
}

} // namespace nicole