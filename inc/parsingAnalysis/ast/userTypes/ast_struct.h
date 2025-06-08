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
  mutable std::shared_ptr<Type> fatherType_;
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

  [[nodiscard]] const std::string &id() const noexcept { return id_; }

  [[nodiscard]] const std::vector<GenericParameter> &generics() const noexcept {
    return generics_;
  }

  void
  setGenerics(const std::vector<GenericParameter> &generics) const noexcept {
    generics_ = generics;
  }

  [[nodiscard]] const std::shared_ptr<Type> &fatherType() const noexcept {
    return fatherType_;
  }

  [[nodiscard]] const std::string &mangledId() const noexcept {
    return mangledId_;
  }

  void setFatherType(const std::shared_ptr<Type> &fatherType) const noexcept {
    fatherType_ = fatherType;
  }

  [[nodiscard]] const Attributes &attributes() const noexcept {
    return attributes_;
  }

  void setAttributes(const Attributes &attributes) const noexcept {
    attributes_ = attributes;
  }

  [[nodiscard]] const std::vector<std::shared_ptr<AST_METHOD_DECL>> &
  methods() const noexcept {
    return methods_;
  }

  void setMethods(const std::vector<std::shared_ptr<AST_METHOD_DECL>> &methods)
      const noexcept {
    methods_ = methods;
  }

  [[nodiscard]] const std::shared_ptr<AST_CONSTRUCTOR_DECL> &
  constructor() const noexcept {
    return constructor_;
  }

  void setConstructor(
      const std::shared_ptr<AST_CONSTRUCTOR_DECL> &constructor) const noexcept {
    constructor_ = constructor;
  }

  [[nodiscard]] const std::shared_ptr<AST_DESTRUCTOR_DECL> &
  destructor() const noexcept {
    return destructor_;
  }

  void setDestructor(
      const std::shared_ptr<AST_DESTRUCTOR_DECL> &destructor) const noexcept {
    destructor_ = destructor;
  }

  // Métodos accept existentes...
  [[nodiscard]] std::expected<std::string, Error>
  accept(const PrintTree &visitor) const noexcept override {
    return visitor.visit(this);
  }

  [[nodiscard]] std::expected<bool, Error>
  accept(const ValidateTree &visitor) const noexcept override {
    return visitor.visit(this);
  }

  [[nodiscard]] std::expected<std::monostate, Error>
  accept(const FillSemanticInfo &visitor) const noexcept override {
    return visitor.visit(this);
  }

  [[nodiscard]] std::expected<std::shared_ptr<Type>, Error>
  accept(const TypeAnalysis &visitor) const noexcept override {
    return visitor.visit(this);
  }

  [[nodiscard]] std::expected<std::monostate, Error>
  accept(const Monomorphize &visitor) const noexcept override {
    return visitor.visit(this);
  }

  [[nodiscard]] std::expected<llvm::Value*, Error>
  accept(const CodeGeneration &visitor) const noexcept override {
    return visitor.visit(this);
  }

  void setMangledId(const std::string &id) const noexcept { mangledId_ = id; }
};

} // namespace nicole

#endif
