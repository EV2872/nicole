#ifndef AST_CHAR_H
#define AST_CHAR_H

#include "../ast.h"
#include <string>

namespace nicole {

class AST_CHAR final : public AST {
private:
  char value_;

public:
  explicit AST_CHAR(const long long unsigned nodeId,
                    const SourceLocation &srcLoc, const char value) noexcept
      : AST{nodeId, AST_TYPE::CHAR, srcLoc}, value_{value} {}

  ~AST_CHAR() noexcept = default;

  [[nodiscard]] auto value() const noexcept -> char { return value_; }

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
