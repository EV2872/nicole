#ifndef AST_BOOL_H
#define AST_BOOL_H

#include "../ast.h"

namespace nicole {

class AST_BOOL final : public AST {
private:
  bool value_;

public:
  explicit AST_BOOL(const long long unsigned nodeId,
                    const SourceLocation &srcLoc, const bool value) noexcept
      : AST{nodeId, AST_TYPE::BOOL, srcLoc}, value_{value} {}

  ~AST_BOOL() noexcept = default;

  [[nodiscard]] bool value() const noexcept { return value_; }

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
