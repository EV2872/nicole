#ifndef AST_SWITCH_H
#define AST_SWITCH_H

#include "ast_case.h"
#include "ast_condition.h"
#include "ast_default.h"
#include <memory>

namespace nicole {

class AST_SWITCH : public AST {
private:
  std::shared_ptr<AST_CONDITION> condition_;
  std::vector<std::shared_ptr<AST_CASE>> cases_;
  std::shared_ptr<AST_DEFAULT> default_;

public:
  explicit AST_SWITCH(const long long unsigned nodeId,
                      const SourceLocation &srcLoc,
                      const std::shared_ptr<AST_CONDITION> &condition,
                      const std::vector<std::shared_ptr<AST_CASE>> &cases,
                      const std::shared_ptr<AST_DEFAULT> &default__) noexcept
      : AST(nodeId, AST_TYPE::SWITCH, srcLoc), condition_{condition},
        cases_{cases}, default_{default__} {}

  [[nodiscard]] auto condition() const noexcept
      -> const std::shared_ptr<AST_CONDITION> & {
    return condition_;
  }

  [[nodiscard]] auto cases() const noexcept
      -> const std::vector<std::shared_ptr<AST_CASE>> & {
    return cases_;
  }

  [[nodiscard]] auto defaultCase() const noexcept
      -> const std::shared_ptr<AST_DEFAULT> & {
    return default_;
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