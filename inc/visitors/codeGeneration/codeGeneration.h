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
#include <variant>
#include <vector>

namespace nicole {

class CodeGeneration final : public Visitor<llvm::Value *> {
private:
  std::shared_ptr<FunctionTable> functionTable_;
  std::shared_ptr<TypeTable> typeTable_;
  Options options_;
  // Start LLVM
  mutable llvm::LLVMContext context_;
  mutable llvm::IRBuilder<> builder_{context_};

  // 2) Módulo, lo gestionamos con unique_ptr para que se limpie al final
  mutable std::unique_ptr<llvm::Module> module_{
      std::make_unique<llvm::Module>("my_module", context_)};

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

  [[nodiscard]] std::expected<std::string, Error>
  nameMangling(const std::shared_ptr<Type> &type) const noexcept;

  [[nodiscard]] std::expected<std::string, Error>
  nameManglingImpl(const std::shared_ptr<Type> &type,
                   std::string &result) const noexcept;

  [[nodiscard]] std::expected<std::string, Error>
  nameManglingFunction(const Function &func,
                       const std::vector<std::shared_ptr<Type>>
                           &genericReplacements) const noexcept;

  [[nodiscard]] std::expected<std::string, Error> nameManglingFunctionImpl(
      const Function &func,
      const std::vector<std::shared_ptr<Type>> &genericReplacements,
      std::string &result) const noexcept;

  [[nodiscard]] std::expected<std::string, Error>
  nameManglingFunctionDecl(const Function &func) const noexcept;

  // Devuelve la dirección (lvalue) de la expresión AST.
  std::expected<llvm::Value *, Error>
  emitLValue(const AST *node) const noexcept;

  // Devuelve el valor cargado (rvalue) de la expresión AST.
  std::expected<llvm::Value *, Error>
  emitRValue(const AST *node) const noexcept;

  [[nodiscard]] std::expected<std::monostate, Error>
  emitObjectFile(llvm::Module *module) const noexcept;

  mutable std::unordered_set<llvm::Value *> allocatedPtrs_;
  mutable llvm::Function *mallocFn_ = nullptr;
  mutable llvm::Function *freeFn_ = nullptr;
  mutable llvm::FunctionCallee strdupFn_; // copy strings
  void ensureMallocFreeDeclared() const noexcept;

  bool isAggregateReturn(const AST_FUNC_DECL *node) const;

  mutable bool isMain{true};

public:
  CodeGeneration(const std::shared_ptr<FunctionTable> &functionTable,
                 std::shared_ptr<TypeTable> &typeTable,
                 const Options &options) noexcept
      : functionTable_{functionTable}, typeTable_{typeTable},
        options_(options) {}

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_BOOL *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_CHAR *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_DOUBLE *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_FLOAT *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_INT *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_NULL *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_STRING *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_VECTOR *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_INDEX *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_DELETE *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_NEW *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_DEREF *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_BINARY *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_UNARY *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_ASSIGNMENT *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_PRINT *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_IMPORT *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_STATEMENT *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_BODY *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_WHILE *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_FOR *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_DO_WHILE *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_PASS *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_STOP *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_IF *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_ELSE_IF *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_SWITCH *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_CASE *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_DEFAULT *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_TERNARY *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_CONDITION *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_FUNC_CALL *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_FUNC_DECL *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_RETURN *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_ENUM *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_ENUM_ACCESS *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_STRUCT *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_ATTR_ACCESS *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_METHOD_CALL *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_METHOD_DECL *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_CONSTRUCTOR_DECL *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_SUPER *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_DESTRUCTOR_DECL *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_THIS *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_CONSTRUCTOR_CALL *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_AUTO_DECL *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_VAR_TYPED_DECL *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_VAR_CALL *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const AST_CHAINED *node) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  visit(const Tree *tree) const noexcept override;

  [[nodiscard]] std::expected<llvm::Value *, Error>
  generate(const Tree *tree) const noexcept {
    return visit(tree);
  }
};

} // namespace nicole

#endif