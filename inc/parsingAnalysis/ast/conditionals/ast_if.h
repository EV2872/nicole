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

  [[nodiscard]] const std::shared_ptr<AST_CONDITION> &
  condition() const noexcept {
    return condition_;
  }

  [[nodiscard]] const std::shared_ptr<AST_BODY> &body() const noexcept {
    return body_;
  }

  [[nodiscard]] const std::vector<std::shared_ptr<AST_ELSE_IF>> &
  elseIf() const noexcept {
    return elseIf_;
  }

  [[nodiscard]] const std::shared_ptr<AST_BODY> &elseBody() const noexcept {
    return elseBody_;
  }

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
};

} // namespace nicole

#endif