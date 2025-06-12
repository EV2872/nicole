#ifndef AST_VAR_CALL_H
#define AST_VAR_CALL_H

#include "../ast.h"
#include <string>

namespace nicole {

class AST_VAR_CALL final : public AST {
private:
  std::string id_;

public:
  explicit AST_VAR_CALL(const long long unsigned nodeId,
                        const SourceLocation &srcLoc,
                        const std::string &id) noexcept
      : AST(nodeId, AST_TYPE::VAR_CALL, srcLoc), id_{id} {}

  [[nodiscard]] auto id() const noexcept -> const std::string & { return id_; }

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