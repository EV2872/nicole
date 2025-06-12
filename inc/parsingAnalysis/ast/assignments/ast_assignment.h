#ifndef AST_ASSIGNMENT_H
#define AST_ASSIGNMENT_H

#include "../../../lexicalAnalysis/token.h"
#include "../ast.h"
#include <string>

namespace nicole {

class AST_ASSIGNMENT final : public AST {
private:
  Token op_;
  std::shared_ptr<AST> left_;
  std::shared_ptr<AST> value_;

public:
  explicit AST_ASSIGNMENT(const long long unsigned nodeId,
                          const SourceLocation &srcLoc, const Token &op,
                          const std::shared_ptr<AST> &left,
                          const std::shared_ptr<AST> &value) noexcept
      : AST(nodeId, AST_TYPE::ASSIGNMENT, srcLoc), op_{op}, left_{left},
        value_{value} {}

  [[nodiscard]] auto op() const noexcept -> const Token & { return op_; }

  [[nodiscard]] auto left() const noexcept -> const std::shared_ptr<AST> & {
    return left_;
  }

  [[nodiscard]] auto value() const noexcept -> const std::shared_ptr<AST> & {
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

#endif