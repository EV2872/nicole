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

  [[nodiscard]] bool isConst() const noexcept { return isConst_; }

  [[nodiscard]] std::expected<std::string, Error>
  accept(const PrintTree &visitor) const noexcept override {
    return visitor.visit(this);
  }

  [[nodiscard]] std::expected<bool, Error>
  accept(const ValidateTree &visitor) const noexcept override {
    return visitor.visit(this);
  }

  [[nodiscard]] std::expected<std::monostate, Error>
  accept(const FillSemanticInfo &visitor) const noexcept override {
    return visitor.visit(this);
  }

  [[nodiscard]] std::expected<std::shared_ptr<Type>, Error>
  accept(const TypeAnalysis &visitor) const noexcept override {
    return visitor.visit(this);
  }

  [[nodiscard]] std::expected<std::monostate, Error>
  accept(const Monomorphize &visitor) const noexcept override {
    return visitor.visit(this);
  }

  [[nodiscard]] std::expected<llvm::Value*, Error>
  accept(const CodeGeneration &visitor) const noexcept override {
    return visitor.visit(this);
  }
};

} // namespace nicole

#endif