#ifndef AST_AUTO_DECL_H
#define AST_AUTO_DECL_H

#include "ast_varDecl.h"

namespace nicole {

class AST_AUTO_DECL : public AST_VAR_DECL {
private:
  bool isConst_;

public:
  explicit AST_AUTO_DECL(const long long unsigned nodeId,
                         const SourceLocation &srcLoc, const std::string &id,
                         const std::shared_ptr<AST> &value,
                         const bool isConst) noexcept
      : AST_VAR_DECL(nodeId, AST_TYPE::AUTO_DECL, srcLoc, id, value),
        isConst_{isConst} {}

  [[nodiscard]] auto isConst() const noexcept -> bool { return isConst_; }

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