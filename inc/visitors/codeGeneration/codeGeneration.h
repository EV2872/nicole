#ifndef CODE_GENERATION_H
#define CODE_GENERATION_H

#include "../../options/options.h"
#include "../../tables/functionTable/functionTable.h"
#include "../../tables/scope/scope.h"
#include "../../tables/typeTable/typeTable.h"
#include "../visitor.h"
#include <expected>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <stack>
#include <unordered_set>
#include <vector>

namespace nicole {

class CodeGeneration final : public Visitor<llvm::Value *> {
private:
  std::shared_ptr<FunctionTable> functionTable_;
  std::shared_ptr<TypeTable> typeTable_;
  Options options_;
  // Start LLVM
  mutable std::unique_ptr<llvm::LLVMContext> context_{
      std::make_unique<llvm::LLVMContext>()};
  mutable llvm::IRBuilder<> builder_{*context_};

  // 2) Módulo, lo gestionamos con unique_ptr para que se limpie al final
  mutable std::unique_ptr<llvm::Module> module_{
      std::make_unique<llvm::Module>("my_module", *context_)};

  // 3) Función main y bloque básico, punteros no propietarios
  mutable llvm::FunctionType *funcType_{nullptr};
  mutable llvm::Function *mainFunction_{nullptr};
  mutable llvm::BasicBlock *entry_{nullptr};
  mutable llvm::Value *resultChainedExpression_{nullptr};
  mutable std::shared_ptr<Type> currentType{nullptr};
  mutable llvm::BasicBlock *currentMergeBlock_{nullptr};
  mutable std::stack<llvm::BasicBlock *> continueTargets_;
  mutable std::stack<llvm::BasicBlock *> breakTargets_;

  mutable std::shared_ptr<Scope> currentScope_{nullptr};

  mutable bool mustLoad{false};
  mutable bool insideStruct{false};
  mutable std::shared_ptr<UserType> currentUserType_{nullptr};

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
  nameManglingFunctionDecl(const Function &func) const noexcept
      -> std::expected<std::string, Error>;

  // Devuelve la dirección (lvalue) de la expresión AST.
  [[nodiscard]] auto emitLValue(const AST *node) const noexcept
      -> std::expected<llvm::Value *, Error>;

  // Devuelve el valor cargado (rvalue) de la expresión AST.
  [[nodiscard]] auto emitRValue(const AST *node) const noexcept
      -> std::expected<llvm::Value *, Error>;

  mutable std::unordered_set<llvm::Value *> allocatedPtrs_;
  mutable llvm::Function *mallocFn_ = nullptr;
  mutable llvm::Function *freeFn_ = nullptr;
  mutable llvm::FunctionCallee strdupFn_; // copy strings
  auto ensureMallocFreeDeclared() const noexcept -> void;

  [[nodiscard]] auto isAggregateReturn(const AST_FUNC_DECL *node) const noexcept
      -> bool;

  mutable bool isMain{true};

  mutable llvm::Value *sretPtr_{nullptr};

public:
  CodeGeneration(const std::shared_ptr<FunctionTable> &functionTable,
                 std::shared_ptr<TypeTable> &typeTable,
                 const Options &options) noexcept
      : functionTable_{functionTable}, typeTable_{typeTable},
        options_(options) {}

  [[nodiscard]] auto visit(const AST_BOOL *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_CHAR *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_DOUBLE *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_FLOAT *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_INT *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_NULL *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_STRING *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_VECTOR *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_INDEX *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_DELETE *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_NEW *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_DEREF *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_BINARY *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_UNARY *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_ASSIGNMENT *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_PRINT *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_IMPORT *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_STATEMENT *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_BODY *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_WHILE *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_FOR *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_DO_WHILE *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_PASS *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_STOP *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_IF *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_ELSE_IF *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_SWITCH *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_CASE *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_DEFAULT *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_TERNARY *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_CONDITION *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_FUNC_CALL *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_FUNC_DECL *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_RETURN *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_ENUM *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_ENUM_ACCESS *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_STRUCT *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_ATTR_ACCESS *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_METHOD_CALL *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_METHOD_DECL *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_CONSTRUCTOR_DECL *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_SUPER *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_DESTRUCTOR_DECL *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_THIS *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_CONSTRUCTOR_CALL *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_AUTO_DECL *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_VAR_TYPED_DECL *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_VAR_CALL *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const AST_CHAINED *node) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto visit(const Tree *tree) const noexcept
      -> std::expected<llvm::Value *, Error> override;

  [[nodiscard]] auto generate(const Tree *tree) const noexcept
      -> std::expected<llvm::orc::ThreadSafeModule, Error>;
};

} // namespace nicole

#endif