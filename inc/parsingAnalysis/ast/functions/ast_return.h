#ifndef AST_RETURN_H
#define AST_RETURN_H

#include "../ast.h"
#include <memory>

namespace nicole {

class AST_RETURN : public AST {
private:
  std::shared_ptr<AST> expression_;

public:
  explicit AST_RETURN(const long long unsigned nodeId,
                      const SourceLocation &srcLoc,
                      const std::shared_ptr<AST> &expression) noexcept
      : AST(nodeId, AST_TYPE::RETURN, srcLoc), expression_{expression} {}

  [[nodiscard]] auto expression() const noexcept
      -> const std::shared_ptr<AST> & {
    return expression_;
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