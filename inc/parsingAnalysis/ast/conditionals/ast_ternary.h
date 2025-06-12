#ifndef AST_TERNARY_H
#define AST_TERNARY_H

#include "ast_condition.h"
#include <memory>

namespace nicole {

class AST_TERNARY : public AST {
private:
  std::shared_ptr<AST_CONDITION> condition_;
  std::shared_ptr<AST> first_;
  std::shared_ptr<AST> second_;

public:
  explicit AST_TERNARY(const long long unsigned nodeId,
                       const SourceLocation &srcLoc,
                       const std::shared_ptr<AST_CONDITION> &condition,
                       const std::shared_ptr<AST> &first,
                       const std::shared_ptr<AST> &second) noexcept
      : AST(nodeId, AST_TYPE::TERNARY, srcLoc), condition_{condition},
        first_{first}, second_{second} {}

  [[nodiscard]] auto condition() const noexcept
      -> const std::shared_ptr<AST_CONDITION> & {
    return condition_;
  }

  [[nodiscard]] auto first() const noexcept -> const std::shared_ptr<AST> & {
    return first_;
  }

  [[nodiscard]] auto second() const noexcept -> const std::shared_ptr<AST> & {
    return second_;
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