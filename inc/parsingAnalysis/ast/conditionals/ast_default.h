#ifndef AST_DEFAULT_H
#define AST_DEFAULT_H

#include "../statements/ast_body.h"
#include <memory>

namespace nicole {

class AST_DEFAULT : public AST {
private:
  std::shared_ptr<AST_BODY> body_;

public:
  explicit AST_DEFAULT(const long long unsigned nodeId,
                       const SourceLocation &srcLoc,
                       const std::shared_ptr<AST_BODY> &body) noexcept
      : AST(nodeId, AST_TYPE::DEFAULT, srcLoc), body_{body} {}

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