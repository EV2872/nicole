#ifndef AST_STRUCT_H
#define AST_STRUCT_H

#include "ast_constructorDecl.h"
#include "ast_destructorDecl.h"
#include "ast_methodDecl.h"
#include "attributes.h"
#include <memory>
#include <vector>

namespace nicole {

class AST_STRUCT final : public AST {
private:
  std::string id_;
  mutable std::vector<GenericParameter> generics_;
  mutable std::weak_ptr<Type> fatherType_;
  mutable Attributes attributes_;
  mutable std::vector<std::shared_ptr<AST_METHOD_DECL>> methods_;
  mutable std::shared_ptr<AST_CONSTRUCTOR_DECL> constructor_;
  mutable std::shared_ptr<AST_DESTRUCTOR_DECL> destructor_;
  mutable std::string mangledId_;

public:
  explicit AST_STRUCT(
      const long long unsigned nodeId, const SourceLocation &srcLoc,
      const std::string &id, const std::vector<GenericParameter> &generics,
      const std::shared_ptr<Type> &fatherType, const Attributes &attributes,
      const std::vector<std::shared_ptr<AST_METHOD_DECL>> &methods,
      const std::shared_ptr<AST_CONSTRUCTOR_DECL> &constructor,
      const std::shared_ptr<AST_DESTRUCTOR_DECL> &destructor) noexcept
      : AST(nodeId, AST_TYPE::STRUCT_DECL, srcLoc), id_{id},
        generics_{generics}, fatherType_{fatherType}, attributes_{attributes},
        methods_{methods}, constructor_{constructor}, destructor_{destructor} {}

  [[nodiscard]] auto id() const noexcept -> const std::string & { return id_; }

  [[nodiscard]] auto generics() const noexcept
      -> const std::vector<GenericParameter> & {
    return generics_;
  }

  auto setGenerics(const std::vector<GenericParameter> &generics) const noexcept
      -> void {
    generics_ = generics;
  }

  [[nodiscard]] auto fatherType() const noexcept
      -> const std::shared_ptr<Type> {
    return fatherType_.lock();
  }

  [[nodiscard]] auto mangledId() const noexcept -> const std::string & {
    return mangledId_;
  }

  auto setFatherType(const std::shared_ptr<Type> &fatherType) const noexcept
      -> void {
    fatherType_ = fatherType;
  }

  [[nodiscard]] auto attributes() const noexcept -> const Attributes & {
    return attributes_;
  }

  auto setAttributes(const Attributes &attributes) const noexcept -> void {
    attributes_ = attributes;
  }

  [[nodiscard]] auto methods() const noexcept
      -> const std::vector<std::shared_ptr<AST_METHOD_DECL>> & {
    return methods_;
  }

  auto setMethods(const std::vector<std::shared_ptr<AST_METHOD_DECL>> &methods)
      const noexcept -> void {
    methods_ = methods;
  }

  [[nodiscard]] auto constructor() const noexcept
      -> const std::shared_ptr<AST_CONSTRUCTOR_DECL> & {
    return constructor_;
  }

  auto setConstructor(const std::shared_ptr<AST_CONSTRUCTOR_DECL> &constructor)
      const noexcept -> void {
    constructor_ = constructor;
  }

  [[nodiscard]] auto destructor() const noexcept
      -> const std::shared_ptr<AST_DESTRUCTOR_DECL> & {
    return destructor_;
  }

  auto setDestructor(const std::shared_ptr<AST_DESTRUCTOR_DECL> &destructor)
      const noexcept -> void {
    destructor_ = destructor;
  }

  [[nodiscard]] auto accept(const PrintTree &visitor) const noexcept
      -> std::expected<std::string, Error> override {
    return visitor.visit(this);
  }

  [[nodiscard]] auto accept(const ValidateTree &visitor) const noexcept
      -> std::expected<bool, Error> override {
    return visitor.visit(this);
  }

  [[nodiscard]] auto accept(const FillSemanticInfo &visitor) const noexcept
      -> std::expected<std::monostate, Error> override {
    return visitor.visit(this);
  }

  [[nodiscard]] auto accept(const TypeAnalysis &visitor) const noexcept
      -> std::expected<std::shared_ptr<Type>, Error> override {
    return visitor.visit(this);
  }

  [[nodiscard]] auto accept(const Monomorphize &visitor) const noexcept
      -> std::expected<std::monostate, Error> override {
    return visitor.visit(this);
  }

  [[nodiscard]] auto accept(const CodeGeneration &visitor) const noexcept
      -> std::expected<llvm::Value *, Error> override {
    return visitor.visit(this);
  }

  void setMangledId(const std::string &id) const noexcept { mangledId_ = id; }
};

} // namespace nicole

#endif
