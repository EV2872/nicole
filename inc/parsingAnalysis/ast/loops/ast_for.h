#ifndef AST_FOR_H
#define AST_FOR_H

#include "ast_loop.h"
#include <memory>

namespace nicole {

class AST_FOR : public AST_LOOP {
private:
  std::vector<std::shared_ptr<AST>> init_;
  std::shared_ptr<AST_CONDITION> condition_;
  std::vector<std::shared_ptr<AST>> update_;

public:
  explicit AST_FOR(const long long unsigned nodeId,
                   const SourceLocation &srcLoc,
                   const std::vector<std::shared_ptr<AST>> &init,
                   const std::shared_ptr<AST_CONDITION> &condition,
                   const std::vector<std::shared_ptr<AST>> &update,
                   const std::shared_ptr<AST_BODY> &body) noexcept
      : AST_LOOP(nodeId, AST_TYPE::FOR, srcLoc, condition, body), init_{init},
        update_{update} {}

  [[nodiscard]] auto init() const noexcept
      -> const std::vector<std::shared_ptr<AST>> & {
    return init_;
  }

  [[nodiscard]] auto update() const noexcept
      -> const std::vector<std::shared_ptr<AST>> & {
    return update_;
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