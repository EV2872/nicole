#ifndef AST_CHAINED_H
#define AST_CHAINED_H

#include "../ast.h"
#include <memory>
#include <vector>

namespace nicole {

class AST_CHAINED final : public AST {
private:
  std::shared_ptr<AST> base_;
  std::vector<std::shared_ptr<AST>> operations_;

public:
  explicit AST_CHAINED(
      const long long unsigned nodeId, const SourceLocation &srcLoc,
      const std::shared_ptr<AST> &base,
      const std::vector<std::shared_ptr<AST>> &operations) noexcept
      : AST{nodeId, AST_TYPE::CHAIN, srcLoc}, base_{base},
        operations_{operations} {}

  [[nodiscard]] auto base() const noexcept -> const std::shared_ptr<AST> & {
    return base_;
  }

  [[nodiscard]] auto operations() const noexcept
      -> const std::vector<std::shared_ptr<AST>> & {
    return operations_;
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
