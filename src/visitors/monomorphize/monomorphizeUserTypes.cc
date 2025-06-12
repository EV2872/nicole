#include "../../../inc/parsingAnalysis/ast/userTypes/ast_attrAccess.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_constructorCall.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_methodCall.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_struct.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_this.h"
#include "../../../inc/visitors/monomorphize/monomorphize.h"
#include <memory>
#include <variant>

namespace nicole {

/*
Needs to be monomporhized, included attributes

Primero monomorfizo el padre
Segundo los atributos
Tercero el constructor
Cuarto los metodos
*/
auto Monomorphize::visit(const AST_STRUCT *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_STRUCT");
  }
  if (node->generics().size()) {
    structDeclReferences[node->id()] = std::make_shared<AST_STRUCT>(*node);
  }
  const auto constructor{node->constructor()->accept(*this)};
  if (!constructor) {
    return createError(constructor.error());
  }
  const auto destructor{node->destructor()->accept(*this)};
  if (!destructor) {
    return createError(destructor.error());
  }
  for (const auto &chain : node->methods()) {
    const auto result{chain->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return {};
}

/*
Needs to be monomporhized
*/
auto Monomorphize::visit(const AST_ATTR_ACCESS *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ATTR_ACCESS");
  }
  return {};
}

/*
Needs to be monomporhized
*/
auto Monomorphize::visit(const AST_METHOD_CALL *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_METHOD_CALL");
  }
  for (const auto &chain : node->parameters()) {
    const auto result{chain->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return {};
}

auto Monomorphize::visit(const AST_METHOD_DECL *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_METHOD_DECL");
  }
  const auto result{node->body()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return {};
}

auto Monomorphize::visit(const AST_CONSTRUCTOR_DECL *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_CONSTRUCTOR_DECL");
  }
  if (node->super()) {
    const auto result{node->super()->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  const auto result{node->body()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return {};
}

/*
Needs to be monomporhized
*/
auto Monomorphize::visit(const AST_SUPER *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_SUPER");
  }
  for (const auto &chain : node->arguments()) {
    const auto result{chain->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return {};
}

auto Monomorphize::visit(const AST_DESTRUCTOR_DECL *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_DESTRUCTOR_DECL");
  }
  const auto result{node->body()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return {};
}

auto Monomorphize::visit(const AST_THIS *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_THIS");
  }
  return {};
}

/*
Needs to be monomporhized
*/
auto Monomorphize::visit(const AST_CONSTRUCTOR_CALL *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_CONSTRUCTOR_CALL");
  }
  for (const auto &chain : node->parameters()) {
    const auto result{chain->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return {};
}

} // namespace nicole