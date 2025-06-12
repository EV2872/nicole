#ifndef AST_BINARY_H
#define AST_BINARY_H

#include "../../../lexicalAnalysis/token.h"
#include "../ast.h"
#include <memory>

namespace nicole {

class AST_BINARY : public AST {
private:
  Token op_;
  std::shared_ptr<AST> left_;
  std::shared_ptr<AST> right_;

public:
  explicit AST_BINARY(const long long unsigned nodeId,
                      const SourceLocation &srcLoc, const Token &op,
                      const std::shared_ptr<AST> &left,
                      const std::shared_ptr<AST> &right) noexcept
      : AST(nodeId, AST_TYPE::BINARY, srcLoc), op_{op}, left_{left},
        right_{right} {}

  [[nodiscard]] auto op() const noexcept -> const Token & { return op_; }

  [[nodiscard]] auto left() const noexcept -> const std::shared_ptr<AST> & {
    return left_;
  }

  [[nodiscard]] auto right() const noexcept -> const std::shared_ptr<AST> & {
    return right_;
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