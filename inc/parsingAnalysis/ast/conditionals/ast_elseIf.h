#ifndef AST_ELSE_IF_H
#define AST_ELSE_IF_H

#include "../statements/ast_body.h"
#include "ast_condition.h"
#include <memory>

namespace nicole {

class AST_ELSE_IF : public AST {
private:
  std::shared_ptr<AST_CONDITION> condition_;
  std::shared_ptr<AST_BODY> body_;

public:
  explicit AST_ELSE_IF(const long long unsigned nodeId,
                       const SourceLocation &srcLoc,
                       const std::shared_ptr<AST_CONDITION> &condition,
                       const std::shared_ptr<AST_BODY> &body) noexcept
      : AST(nodeId, AST_TYPE::ELSE_IF, srcLoc), condition_{condition},
        body_{body} {}

  [[nodiscard]] auto condition() const noexcept
      -> const std::shared_ptr<AST_CONDITION> & {
    return condition_;
  }

  [[nodiscard]] auto body() const noexcept
      -> const std::shared_ptr<AST_BODY> & {
    return body_;
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