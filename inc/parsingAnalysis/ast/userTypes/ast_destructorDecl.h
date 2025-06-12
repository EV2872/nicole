#ifndef AST_DESTRUCTOR_DECL_H
#define AST_DESTRUCTOR_DECL_H

#include "../../../tables/typeTable/types/specialTypes/voidType.h"
#include "../functions/ast_subrutineDecl.h"
#include <memory>

namespace nicole {

class AST_DESTRUCTOR_DECL final : public AST_SUBRUTINE_DECL {
private:
  bool isVirtual_;

public:
  explicit AST_DESTRUCTOR_DECL(const long long unsigned nodeId,
                               const SourceLocation &srcLoc,
                               const std::string &id,
                               const std::shared_ptr<Type> &returnType,
                               const std::shared_ptr<AST_BODY> &body,
                               const bool isVirtual) noexcept
      : AST_SUBRUTINE_DECL(nodeId, AST_TYPE::DESTRUCTOR_DECL, srcLoc, id,
                           returnType, body),
        isVirtual_{isVirtual} {}

  [[nodiscard]] auto isVirtual() const noexcept -> bool { return isVirtual_; }

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