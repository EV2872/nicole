#ifndef FILL_SEMANTIC_INFO_H
#define FILL_SEMANTIC_INFO_H

#include "../../options/options.h"
#include "../../tables/functionTable/functionTable.h"
#include "../../tables/scope/scope.h"
#include "../../tables/typeTable/typeTable.h"
#include "../visitor.h"
#include <memory>
#include <variant>
#include <vector>

namespace nicole {

class FillSemanticInfo final : public Visitor<std::monostate> {
private:
  mutable std::shared_ptr<FunctionTable> functionTable_;
  mutable std::shared_ptr<TypeTable> typeTable_;
  Options options_;

  mutable std::shared_ptr<Scope> currentScope_{nullptr};
  mutable std::shared_ptr<Scope> firstScope_{nullptr};
  mutable std::vector<GenericParameter> currentGenericList_{};
  mutable std::vector<GenericParameter> currentStructGenericList_{};
  mutable std::shared_ptr<UserType> currentUserType_{nullptr};
  mutable bool analyzingInsideClass{false};

  auto pushScope() const noexcept -> void;

  auto popScope() const noexcept -> void;

  [[nodiscard]] auto
  mergeGenericLists(const std::vector<GenericParameter> &list1,
                    const std::vector<GenericParameter> &list2) const noexcept
      -> std::expected<std::vector<GenericParameter>, Error>;

  [[nodiscard]] auto hasDuplicatedGenerics(
      const std::vector<GenericParameter> &list) const noexcept -> bool;

  [[nodiscard]] auto
  areAmbiguousFunctions(const Function &first,
                        const Function &second) const noexcept -> bool;

  [[nodiscard]] auto areAmbiguousMethods(const Method &first,
                                         const Method &second) const noexcept
      -> bool;

public:
  FillSemanticInfo(const std::shared_ptr<FunctionTable> &functionTable,
                   std::shared_ptr<TypeTable> &typeTable,
                   const Options &options) noexcept
      : functionTable_{functionTable}, typeTable_{typeTable},
        options_{options} {}

  [[nodiscard]] auto getGlobalScope() const noexcept
      -> const std::shared_ptr<Scope> {
    return firstScope_;
  }

  [[nodiscard]] auto visit(const AST_BOOL *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_CHAR *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_DOUBLE *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_FLOAT *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_INT *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_NULL *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_STRING *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_VECTOR *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_INDEX *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_DELETE *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_NEW *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_DEREF *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_BINARY *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_UNARY *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_ASSIGNMENT *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_PRINT *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_IMPORT *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_STATEMENT *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_BODY *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_WHILE *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_FOR *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_DO_WHILE *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_PASS *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_STOP *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_IF *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_ELSE_IF *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_SWITCH *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_CASE *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_DEFAULT *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_TERNARY *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_CONDITION *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_FUNC_CALL *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_FUNC_DECL *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_RETURN *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_ENUM *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_ENUM_ACCESS *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_STRUCT *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_ATTR_ACCESS *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_METHOD_CALL *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_METHOD_DECL *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_CONSTRUCTOR_DECL *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_SUPER *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_DESTRUCTOR_DECL *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_THIS *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_CONSTRUCTOR_CALL *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_AUTO_DECL *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_VAR_TYPED_DECL *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_VAR_CALL *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const AST_CHAINED *node) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto visit(const Tree *tree) const noexcept
      -> std::expected<std::monostate, Error> override;

  [[nodiscard]] auto fill(const Tree *tree) const noexcept
      -> std::expected<std::monostate, Error> {
    return visit(tree);
  }
};

} // namespace nicole

#endif