#ifndef AST_FLOAT_H
#define AST_FLOAT_H

#include "../ast.h"

namespace nicole {

class AST_FLOAT final : public AST {
private:
  float value_;

public:
  explicit AST_FLOAT(const long long unsigned nodeId,
                     const SourceLocation &srcLoc, const float value) noexcept
      : AST{nodeId, AST_TYPE::FLOAT, srcLoc}, value_{value} {}

  ~AST_FLOAT() noexcept = default;

  [[nodiscard]] float value() const noexcept { return value_; }

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

#endif // AST_H
