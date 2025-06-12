#ifndef PRINT_TREE_H
#define PRINT_TREE_H

#include "../visitor.h"

namespace nicole {

class PrintTree final : public Visitor<std::string> {
private:
  mutable std::string indent_{};

  auto increaseIndent() const noexcept -> void { indent_ += "  "; }

  auto decreaseIndent() const noexcept -> void {
    if (indent_.size() >= 2)
      indent_ = indent_.substr(0, indent_.size() - 2);
  }

public:
  [[nodiscard]] auto visit(const AST_BOOL *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_CHAR *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_DOUBLE *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_FLOAT *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_INT *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_NULL *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_STRING *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_VECTOR *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_INDEX *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_DELETE *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_NEW *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_DEREF *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_BINARY *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_UNARY *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_ASSIGNMENT *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_PRINT *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_IMPORT *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_STATEMENT *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_BODY *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_WHILE *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_FOR *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_DO_WHILE *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_PASS *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_STOP *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_IF *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_ELSE_IF *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_SWITCH *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_CASE *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_DEFAULT *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_TERNARY *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_CONDITION *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_FUNC_CALL *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_FUNC_DECL *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_RETURN *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_ENUM *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_ENUM_ACCESS *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_STRUCT *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_ATTR_ACCESS *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_METHOD_CALL *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_METHOD_DECL *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_CONSTRUCTOR_DECL *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_SUPER *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_DESTRUCTOR_DECL *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_THIS *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_CONSTRUCTOR_CALL *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_AUTO_DECL *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_VAR_TYPED_DECL *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_VAR_CALL *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const AST_CHAINED *node) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto visit(const Tree *tree) const noexcept
      -> std::expected<std::string, Error> override;

  [[nodiscard]] auto print(const Tree *tree) const noexcept
      -> std::expected<std::string, Error> {
    return visit(tree);
  }
};

} // namespace nicole

#endif