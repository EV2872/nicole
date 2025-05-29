#include "../../../inc/parsingAnalysis/ast/variables/ast_autoDecl.h"
#include "../../../inc/parsingAnalysis/ast/variables/ast_typedDecl.h"
#include "../../../inc/parsingAnalysis/ast/variables/ast_varCall.h"
#include "../../../inc/visitors/codeGeneration/codeGeneration.h"
#include <memory>

namespace nicole {

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_AUTO_DECL *node) const noexcept {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_AUTO_DECL");
  // Generar el valor inicial
  std::expected<llvm::Value *, Error> initOrErr =
      emitRValue(node->value().get());
  if (!initOrErr)
    return createError(initOrErr.error());
  llvm::Value *initVal = *initOrErr;

  // Recuperar la Variable existente del scope
  std::expected<std::shared_ptr<Variable>, Error> varOrErr =
      currentScope_->getVariable(node->id());
  if (!varOrErr)
    return createError(varOrErr.error());
  std::shared_ptr<Variable> varPtr = *varOrErr; // shared_ptr<Variable>
  Variable &var = *varPtr;                      // referencia al objeto real

  // Alloca en entry_ usando builder_
  // Guardamos el punto de inserción actual
  llvm::IRBuilder<>::InsertPointGuard guard(builder_);
  // Movemos a justo después de la etiqueta 'entry'
  // builder_.SetInsertPoint(entry_, entry_->getFirstInsertionPt());
  // builder_.SetInsertPoint(entry_);
  std::expected<llvm::Type *, Error> llvmTyOrErr =
      var.type()->llvmVersion(context_);
  if (!llvmTyOrErr)
    return std::unexpected(llvmTyOrErr.error());
  llvm::Type *llvmTy = *llvmTyOrErr;
  llvm::AllocaInst *alloca = builder_.CreateAlloca(llvmTy, nullptr, node->id());
  var.setAddress(alloca);

  const auto isBasicType{std::dynamic_pointer_cast<BasicType>(var.type())};
  if (isBasicType && isBasicType->baseKind() == BasicKind::Str &&
      initVal->getType()->isPointerTy()) {
    initVal = builder_.CreateCall(strdupFn_, {initVal}, "strdup_call");
  }

  // Si es un struct (aggregate), primero cargamos el valor y luego lo
  // almacenamos
  if (llvmTy->isAggregateType()) {
    // initVal es un ptr a struct, cargamos el struct completo
    llvm::LoadInst *loadedStruct =
        builder_.CreateLoad(llvmTy, initVal, node->id() + "_agg_load");
    builder_.CreateStore(loadedStruct, alloca);
    var.setValue(loadedStruct);
  } else {
    // Escalar o ptr: almacenar directamente
    builder_.CreateStore(initVal, alloca);
    var.setValue(initVal);
  }

  return alloca;
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_VAR_TYPED_DECL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_VAR_TYPED_DECL");
  }
  std::expected<llvm::Value *, Error> initOrErr =
      emitRValue(node->value().get());
  if (!initOrErr)
    return createError(initOrErr.error());
  llvm::Value *initVal = *initOrErr;

  std::expected<std::shared_ptr<Variable>, Error> varOrErr =
      currentScope_->getVariable(node->id());
  if (!varOrErr)
    return createError(varOrErr.error());
  std::shared_ptr<Variable> varPtr = *varOrErr; // shared_ptr<Variable>
  Variable &var = *varPtr;                      // referencia al objeto real

  llvm::IRBuilder<>::InsertPointGuard guard(builder_);
  // builder_.SetInsertPoint(entry_, entry_->getFirstInsertionPt());
  std::expected<llvm::Type *, Error> llvmTyOrErr =
      node->varType()->llvmVersion(context_);
  if (!llvmTyOrErr)
    return std::unexpected(llvmTyOrErr.error());
  llvm::Type *llvmTy = *llvmTyOrErr;
  llvm::AllocaInst *alloca = builder_.CreateAlloca(llvmTy, nullptr, node->id());
  var.setAddress(alloca);

  // Si es un struct (aggregate), primero cargamos el valor y luego lo
  // almacenamos
  if (llvmTy->isAggregateType()) {
    // initVal es un ptr a struct, cargamos el struct completo
    llvm::LoadInst *loadedStruct =
        builder_.CreateLoad(llvmTy, initVal, node->id() + "_agg_load");
    builder_.CreateStore(loadedStruct, alloca);
    var.setValue(loadedStruct);
  } else {
    // Escalar o ptr: almacenar directamente
    builder_.CreateStore(initVal, alloca);
    var.setValue(initVal);
  }

  return alloca;
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_VAR_CALL *node) const noexcept {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_VAR_CALL");
  if (insideStruct && currentUserType_ &&
      currentUserType_->hasAttribute(node->id())) {
    // Recuperar el alloca de 'this'
    auto thisOrErr = currentScope_->getVariable("this");
    if (!thisOrErr)
      return createError(thisOrErr.error());

    auto *thisVal = (*thisOrErr)->address(); // llvm::Value*
    auto *thisAlloca = llvm::cast<llvm::AllocaInst>(thisVal);
    // Cargar el puntero al objeto
    llvm::Value *thisPtr =
        builder_.CreateLoad(thisAlloca->getAllocatedType()->getPointerTo(),
                            thisAlloca, "this.load");

    // Calcular índice del campo en la struct
    unsigned idx = static_cast<unsigned>(
        currentUserType_->getAttribute(node->id())->position());
    llvm::Value *indices[] = {builder_.getInt32(0), builder_.getInt32(idx)};

    // Crear GEP para el campo
    llvm::Value *fieldPtr =
        builder_.CreateInBoundsGEP(*currentUserType_->llvmVersion(context_),
                                   thisPtr, indices, node->id() + ".ptr");
    resultChainedExpression_ = fieldPtr;
    return fieldPtr;
  }

  std::expected<std::shared_ptr<Variable>, Error> varOrErr =
      currentScope_->getVariable(node->id());
  if (!varOrErr)
    return createError(varOrErr.error());

  std::shared_ptr<Variable> varPtr = *varOrErr;
  llvm::AllocaInst *addr = varPtr->address();
  if (!addr)
    return createError(ERROR_TYPE::VALIDATE_TREE, "variable has no address");

  resultChainedExpression_ = addr;
  return addr;
}

} // namespace nicole