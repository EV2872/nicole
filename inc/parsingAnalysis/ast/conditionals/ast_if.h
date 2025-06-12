#ifndef AST_IF_H
#define AST_IF_H

#include "ast_condition.h"
#include "ast_elseIf.h"
#include <memory>
#include <vector>

namespace nicole {

class AST_IF : public AST {
private:
  std::shared_ptr<AST_CONDITION> condition_;
  std::shared_ptr<AST_BODY> body_;
  std::vector<std::shared_ptr<AST_ELSE_IF>> elseIf_;
  std::shared_ptr<AST_BODY> elseBody_;

public:
  explicit AST_IF(const long long unsigned nodeId, const SourceLocation &srcLoc,
                  const std::shared_ptr<AST_CONDITION> &condition,
                  const std::shared_ptr<AST_BODY> &body,
                  const std::vector<std::shared_ptr<AST_ELSE_IF>> &elseIf,
                  const std::shared_ptr<AST_BODY> &elseBody) noexcept
      : AST(nodeId, AST_TYPE::IF, srcLoc), condition_{condition}, body_{body},
        elseIf_{elseIf}, elseBody_{elseBody} {}

  [[nodiscard]] auto condition() const noexcept
      -> const std::shared_ptr<AST_CONDITION> & {
    return condition_;
  }

  [[nodiscard]] auto body() const noexcept
      -> const std::shared_ptr<AST_BODY> & {
    return body_;
  }

  [[nodiscard]] auto elseIf() const noexcept
      -> const std::vector<std::shared_ptr<AST_ELSE_IF>> & {
    return elseIf_;
  }

  [[nodiscard]] auto elseBody() const noexcept
      -> const std::shared_ptr<AST_BODY> & {
    return elseBody_;
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