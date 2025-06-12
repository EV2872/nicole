#ifndef AST_VECTOR_H
#define AST_VECTOR_H

#include "../ast.h"
#include <memory>
#include <variant>
#include <vector>

namespace nicole {

class AST_VECTOR : public AST {
private:
  std::vector<std::shared_ptr<AST>> values_;

public:
  explicit AST_VECTOR(const long long unsigned nodeId,
                      const SourceLocation &srcLoc,
                      const std::vector<std::shared_ptr<AST>> &values) noexcept
      : AST(nodeId, AST_TYPE::VECTOR, srcLoc), values_{values} {}

  [[nodiscard]] auto values() const noexcept
      -> const std::vector<std::shared_ptr<AST>> & {
    return values_;
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