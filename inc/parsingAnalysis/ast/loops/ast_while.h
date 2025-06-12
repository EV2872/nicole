#ifndef AST_WHILE_H
#define AST_WHILE_H

#include "ast_loop.h"
#include <memory>

namespace nicole {

class AST_WHILE : public AST_LOOP {
public:
  explicit AST_WHILE(const long long unsigned nodeId,
                     const SourceLocation &srcLoc,
                     const std::shared_ptr<AST_CONDITION> &condition,
                     const std::shared_ptr<AST_BODY> &body) noexcept
      : AST_LOOP{nodeId, AST_TYPE::WHILE, srcLoc, condition, body} {}

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