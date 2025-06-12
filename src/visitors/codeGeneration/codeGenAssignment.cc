#include "../../../inc/visitors/codeGeneration/codeGeneration.h"

#include "../../../inc/parsingAnalysis/ast/assignments/ast_assignment.h"

#include <memory>

namespace nicole {

auto CodeGeneration::visit(const AST_ASSIGNMENT *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ASSIGNMENT");

  // obtener dirección de LHS
  std::expected<llvm::Value *, Error> addrOrErr =
      emitLValue(node->left().get());
  if (!addrOrErr)
    return createError(addrOrErr.error());
  llvm::Value *addr = *addrOrErr;

  // generar valor RHS
  std::expected<llvm::Value *, Error> valOrErr =
      emitRValue(node->value().get());
  if (!valOrErr)
    return createError(valOrErr.error());
  llvm::Value *val = *valOrErr;

  // ajustar tipo según semántica
  std::shared_ptr<Type> semTy = node->left()->returnedFromTypeAnalysis();

  if (auto basic = std::dynamic_pointer_cast<BasicType>(semTy)) {
    if (basic->baseKind() == BasicKind::Str && val->getType()->isPointerTy()) {
      val = builder_.CreateCall(strdupFn_, {val}, "strdup_call");
    }
  }

  std::expected<llvm::Type *, Error> llvmTyOrErr =
      semTy->llvmVersion(*context_);
  if (!llvmTyOrErr)
    return createError(llvmTyOrErr.error());
  llvm::Type *dstTy = *llvmTyOrErr;
  if (dstTy->isAggregateType() && val->getType()->isPointerTy()) {
    val = builder_.CreateLoad(dstTy, val, "assign_load");
  }
  // Si tras el load el tipo sigue sin coincidir, aplica sext/bitcast
  else if (val->getType() != dstTy) {
    if (val->getType()->isIntegerTy() && dstTy->isIntegerTy())
      val = builder_.CreateSExt(val, dstTy, "assign_sext");
    else
      val = builder_.CreateBitCast(val, dstTy, "assign_cast");
  }

  // emitir store
  builder_.CreateStore(val, addr);

  // actualizar para chaining
  resultChainedExpression_ = val;
  currentType = semTy;
  return val;
}

} // namespace nicole