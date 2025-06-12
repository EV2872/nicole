#include "../../../inc/parsingAnalysis/ast/userTypes/ast_attrAccess.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_constructorCall.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_methodCall.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_struct.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_this.h"
#include "../../../inc/visitors/printTree/printTree.h"
#include <ostream>

namespace nicole {

auto PrintTree::visit(const AST_STRUCT *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_STRUCT");
  }

  std::ostringstream result;
  result << indent_ << "Struct Declaration:\n";
  increaseIndent();

  // Nombre de la estructura
  result << indent_ << "Name: " << node->id() << "\n";
  result << indent_ << "Generics: ";
  for (const auto &generic : node->generics()) {
    result << generic.name() << ", ";
  }
  result << "\n";
  // Tipo padre (si existe)
  if (node->fatherType()) {
    result << indent_ << "Father Type: " << node->fatherType()->toString()
           << "\n";
  } else {
    result << indent_ << "No Father Type\n";
  }

  // Atributos
  result << indent_ << "Attributes:\n";
  increaseIndent();
  for (const auto &[name, type] : node->attributes().params()) {
    result << indent_ << name << " " << type->toString() << "\n";
  }
  decreaseIndent();

  // Métodos
  result << indent_ << "Methods:\n";
  increaseIndent();
  for (const auto &method : node->methods()) {
    const auto methodStr = method->accept(*this);
    if (!methodStr) {
      return createError(methodStr.error());
    }
    result << *methodStr;
  }
  decreaseIndent();

  // Constructor
  if (node->constructor()) {
    increaseIndent();
    const auto ctorStr = node->constructor()->accept(*this);
    if (!ctorStr) {
      return createError(ctorStr.error());
    }
    result << *ctorStr;
    decreaseIndent();
  }

  // Destructor
  if (node->destructor()) {
    increaseIndent();
    const auto dtorStr = node->destructor()->accept(*this);
    if (!dtorStr) {
      return createError(dtorStr.error());
    }
    result << *dtorStr;
    decreaseIndent();
  }

  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_ATTR_ACCESS *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ATTR_ACCESS");
  }
  std::ostringstream result;
  result << indent_ << "attr access:\n";
  increaseIndent();
  result << indent_ << node->id() << "\n";
  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_METHOD_CALL *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_METHOD_CALL");
  }

  std::ostringstream result;
  result << indent_ << "Method Call:\n";
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

auto PrintTree::visit(const AST_METHOD_DECL *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_METHOD_DECL");
  }

  std::ostringstream result;
  result << indent_ << "Method Declaration:\n";
  increaseIndent();
  result << indent_ << "Name: " << node->id() << "\n";
  result << indent_ << "Generics: ";
  for (const auto &generic : node->generics()) {
    result << generic.name() << ", ";
  }
  result << "\n";
  result << indent_ << "Return Type: " << node->returnType()->toString()
         << "\n";
  result << indent_ << "Virtual: " << (node->isVirtual() ? "true" : "false")
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

auto PrintTree::visit(const AST_CONSTRUCTOR_DECL *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_CONSTRUCTOR_DECL");
  }

  std::ostringstream result;
  result << indent_ << "Constructor Decl:\n";
  increaseIndent();
  result << indent_ << "Name: " << node->id() << "\n";
  result << indent_ << "Generics: ";
  for (const auto &generic : node->generics()) {
    result << generic.name() << ", ";
  }
  result << "\n";
  result << indent_ << "Parameters:\n";
  increaseIndent();
  for (const auto &param : node->parameters()) {
    result << indent_ << "Param: " << param.first
           << " type: " << param.second->toString() << "\n";
  }
  decreaseIndent();

  if (node->super()) {
    const auto super{node->super()->accept(*this)};
    if (!super) {
      return createError(super.error());
    }
    result << *super;
  }

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

auto PrintTree::visit(const AST_SUPER *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_SUPER");
  }
  std::ostringstream result;
  result << indent_ << "Super:\n";
  result << indent_ << "Type: " + node->fatherType()->toString() << "\n";
  result << indent_ << "Replacement of generics:\n";
  for (const auto &generic : node->replacements()) {
    result << indent_ << generic->toString() << ", ";
  }
  result << "\n";
  result << indent_ << "Arguments:\n";
  increaseIndent();
  for (const auto &arg : node->arguments()) {
    const auto argStr = arg->accept(*this);
    if (!argStr) {
      return createError(argStr.error());
    }
    result << *argStr;
  }
  decreaseIndent();
  return result.str();
}

auto PrintTree::visit(const AST_DESTRUCTOR_DECL *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_DESTRUCTOR_DECL");
  }

  std::ostringstream result;
  result << indent_ << "Destructor Decl:\n";
  increaseIndent();
  result << indent_ << "Name: " << node->id() << "\n";
  result << indent_ << "Virtual: " << (node->isVirtual() ? "true" : "false")
         << "\n";
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

auto PrintTree::visit(const AST_THIS *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_THIS");
  }
  std::ostringstream result;
  result << indent_ << "this call\n";
  return result.str();
}

auto PrintTree::visit(const AST_CONSTRUCTOR_CALL *node) const noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_CONSTRUCTOR_CALL");
  }

  std::ostringstream result;
  result << indent_ << "Constructor Call:\n";
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

} // namespace nicole