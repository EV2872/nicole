#ifndef AST_METHOD_DECL_H
#define AST_METHOD_DECL_H

#include "../../../tables/typeTable/types/userTypes/genericParameter.h"
#include "../functions/ast_parametrizedSubritineDecl.h"
#include <memory>

namespace nicole {

class AST_METHOD_DECL final : public AST_PARAMETRIZED_SUBRUTINE_DECL {
private:
  mutable std::vector<GenericParameter> generics_;
  bool isVirtual_;

public:
  explicit AST_METHOD_DECL(const long long unsigned nodeId,
                           const SourceLocation &srcLoc, const std::string &id,
                           const std::vector<GenericParameter> &generics,
                           const Parameters &params,
                           const std::shared_ptr<Type> &returnType,
                           const std::shared_ptr<AST_BODY> &body,
                           const bool isVirtual) noexcept
      : AST_PARAMETRIZED_SUBRUTINE_DECL(nodeId, AST_TYPE::METHOD_DECL, srcLoc,
                                        id, params, returnType, body),
        generics_{generics}, isVirtual_{isVirtual} {}

  [[nodiscard]] auto generics() const noexcept
      -> const std::vector<GenericParameter> & {
    return generics_;
  }

  auto setGenerics(const std::vector<GenericParameter> &generics) const noexcept
      -> void {
    generics_ = generics;
  }

  [[nodiscard]] auto isVirtual() const noexcept -> bool { return isVirtual_; }

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
};

} // namespace nicole

#endif