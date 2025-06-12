#ifndef AST_BODY_H
#define AST_BODY_H

#include "../../../tables/scope/scope.h"
#include "ast_statement.h"
#include <memory>
#include <vector>

namespace nicole {

class AST_BODY : public AST {
private:
  std::vector<std::shared_ptr<AST_STATEMENT>> body_;
  mutable std::shared_ptr<Scope> scope_;

public:
  explicit AST_BODY(
      const long long unsigned nodeId, const SourceLocation &srcLoc,
      const std::vector<std::shared_ptr<AST_STATEMENT>> &body) noexcept
      : AST(nodeId, AST_TYPE::BODY, srcLoc), body_{body} {}

  [[nodiscard]] auto body() const noexcept
      -> const std::vector<std::shared_ptr<AST_STATEMENT>> & {
    return body_;
  }

  [[nodiscard]] auto scope() const noexcept -> const std::shared_ptr<Scope> & {
    return scope_;
  }

  auto setScope(const std::shared_ptr<Scope> &scope) const noexcept -> void {
    scope_ = scope;
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
};

} // namespace nicole

#endif