#include "../../../inc/parsingAnalysis/ast/functions/ast_funcCall.h"
#include "../../../inc/parsingAnalysis/ast/functions/ast_funcDecl.h"
#include "../../../inc/parsingAnalysis/ast/functions/ast_return.h"
#include "../../../inc/visitors/printTree/printTree.h"
#include <ostream>

namespace nicole {

auto PrintTree::visit(const AST_FUNC_CALL *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_FUNC_CALL");
  }

  std::ostringstream result;
  result << indent_ << "Function Call:\n";
  increaseIndent();
  result << indent_ << "Name: " << node->id() << "\n";
  result << indent_ << "Replacement of generics:\n" << indent_;
  for (const auto &generic : node->replaceOfGenerics()) {
    result << generic->toString() << ", ";
  }
  result << "\n";
  result << indent_ << "Arguments:\n";
  increaseIndent();
  for (const auto &arg : node->parameters()) {
    const auto argStr = arg->accept(*this);
    if (!argStr) {
      return createError(argStr.error());
    }
    result << *argStr;
  }
  decreaseIndent();
  decreaseIndent();

  return result.str();
}

auto PrintTree::visit(const AST_FUNC_DECL *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_FUNC_DECL");
  }
  std::ostringstream result;
  result << indent_ << "Function Declaration:\n";
  increaseIndent();
  result << indent_ << "Name: " << node->id() << "\n";
  result << indent_ << "Generics: ";
  for (const auto &generic : node->generics()) {
    result << generic.name() << ", ";
  }
  result << "\n";
  result << indent_ << "Return Type: " << node->returnType()->toString()
         << "\n";

  result << indent_ << "Parameters:\n";
  increaseIndent();
  for (const auto &param : node->parameters()) {
    result << indent_ << "Param: " << param.first
           << " type: " << param.second->toString() << "\n";
  }
  decreaseIndent();

  if (node->body()) {
    const auto bodyStr{node->body()->accept(*this)};
    if (!bodyStr) {
      return createError(bodyStr.error());
    }
    result << *bodyStr;
  } else {
    result << indent_ << "(empty body)\n";
  }
  decreaseIndent();

  return result.str();
}

auto PrintTree::visit(const AST_RETURN *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_RETURN");
  }
  std::ostringstream result;
  result << indent_ << "Return:\n";
  increaseIndent();
  if (node->expression()) {
    const auto val{node->expression()->accept(*this)};
    if (!val) {
      return createError(val.error());
    }
    result << indent_ << "value:\n" << *val;
  } else {
    result << indent_ << "empty";
  }
  decreaseIndent();
  return result.str();
}

} // namespace nicole