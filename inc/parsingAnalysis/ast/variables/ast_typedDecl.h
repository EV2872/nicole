#ifndef AST_VAR_TYPED_DECL_H
#define AST_VAR_TYPED_DECL_H

#include "../../../tables/typeTable/types/type.h"
#include "ast_varDecl.h"
#include <memory>

namespace nicole {

class AST_VAR_TYPED_DECL : public AST_VAR_DECL {
private:
  mutable std::shared_ptr<Type> type_;

public:
  explicit AST_VAR_TYPED_DECL(const long long unsigned nodeId,
                              const SourceLocation &srcLoc,
                              const std::string &id,
                              const std::shared_ptr<Type> &type,
                              const std::shared_ptr<AST> &value) noexcept
      : AST_VAR_DECL(nodeId, AST_TYPE::VAR_TYPED_DECL, srcLoc, id, value),
        type_{type} {}

  [[nodiscard]] auto varType() const noexcept -> const std::shared_ptr<Type> & {
    return type_;
  }

  void setVarType(const std::shared_ptr<Type> &type) const noexcept {
    type_ = type;
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