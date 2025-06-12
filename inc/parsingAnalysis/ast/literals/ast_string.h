#ifndef AST_STRING_H
#define AST_STRING_H

#include "../ast.h"

namespace nicole {

class AST_STRING final : public AST {
private:
  std::string value_;

public:
  explicit AST_STRING(const long long unsigned nodeId,
                      const SourceLocation &srcLoc,
                      const std::string value) noexcept
      : AST{nodeId, AST_TYPE::STRING, srcLoc}, value_{value} {}

  ~AST_STRING() noexcept = default;

  [[nodiscard]] auto value() const noexcept -> const std::string & {
    return value_;
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

#endif // AST_H
