#ifndef AST_DOUBLE_H
#define AST_DOUBLE_H

#include "../ast.h"

namespace nicole {

class AST_DOUBLE final : public AST {
private:
  double value_;

public:
  explicit AST_DOUBLE(const long long unsigned nodeId,
                      const SourceLocation &srcLoc, const double value) noexcept
      : AST{nodeId, AST_TYPE::DOUBLE, srcLoc}, value_{value} {}

  ~AST_DOUBLE() noexcept = default;

  [[nodiscard]] auto value() const noexcept -> double { return value_; }

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
