#ifndef MONOMORPHIZE_H
#define MONOMORPHIZE_H

#include "../../tables/functionTable/functionTable.h"
#include "../../tables/scope/scope.h"
#include "../../tables/typeTable/typeTable.h"
#include "../visitor.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace nicole {

class Monomorphize final : public Visitor<std::monostate> {
private:
  mutable std::shared_ptr<FunctionTable> functionTable_;
  mutable std::shared_ptr<TypeTable> typeTable_;
  mutable std::vector<GenericParameter> currentCallGenerics_;
  mutable std::vector<std::shared_ptr<Type>> currentCallReplacements_;
  mutable std::vector<GenericParameter> currentGenericList_{};
  mutable std::vector<GenericParameter> currentStructGenericList_{};
  mutable std::shared_ptr<UserType> currentUserType_{nullptr};
  mutable std::shared_ptr<Type> switchTypeCondition_{nullptr};
  mutable std::unordered_map<std::string,
                             std::vector<std::shared_ptr<AST_FUNC_DECL>>>
      funcDeclReferences;
  mutable std::unordered_set<std::string> specializedFunctions_;
  mutable std::unordered_set<std::string> specializedMethod_;
  mutable std::unordered_map<std::string, std::shared_ptr<AST_STRUCT>>
      structDeclReferences;
  mutable std::shared_ptr<AST_STRUCT> currentMangledStruct{nullptr};
  mutable bool analyzingInsideClass{false};
  mutable bool insideDeclWithGenerics{false};
  mutable bool insideCopy{false};

  [[nodiscard]] auto
  nameMangling(const std::shared_ptr<Type> &type) const noexcept
      -> std::expected<std::string, Error>;

  [[nodiscard]] auto nameManglingImpl(const std::shared_ptr<Type> &type,
                                      std::string &result) const noexcept
      -> std::expected<std::string, Error>;

  [[nodiscard]] auto nameManglingFunction(
      const Function &func,
      const std::vector<std::shared_ptr<Type>> &genericReplacements)
      const noexcept -> std::expected<std::string, Error>;

  [[nodiscard]] auto nameManglingFunctionImpl(
      const Function &func,
      const std::vector<std::shared_ptr<Type>> &genericReplacements,
      std::string &result) const noexcept -> std::expected<std::string, Error>;

  [[nodiscard]] auto
  mergeGenericLists(const std::vector<GenericParameter> &list1,
                    const std::vector<GenericParameter> &list2) const noexcept
      -> std::vector<GenericParameter>;

public:
  Monomorphize(const std::shared_ptr<FunctionTable> &functionTable,
               std::shared_ptr<TypeTable> &typeTable) noexcept
      : functionTable_{functionTable}, typeTable_{typeTable} {}

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

  [[nodiscard]] auto transform(const Tree *tree) const noexcept
      -> std::expected<std::monostate, Error> {
    return visit(tree);
  }
};

} // namespace nicole

#endif