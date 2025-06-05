#include "../../../inc/parsingAnalysis/ast/pointer/ast_delete.h"
#include "../../../inc/parsingAnalysis/ast/chained/ast_chained.h"
#include "../../../inc/parsingAnalysis/ast/pointer/ast_deref.h"
#include "../../../inc/parsingAnalysis/ast/pointer/ast_new.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_constructorCall.h"
#include "../../../inc/visitors/codeGeneration/codeGeneration.h"
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <memory>

namespace nicole {

void CodeGeneration::ensureMallocFreeDeclared() const noexcept {
  if (!mallocFn_) {
    // void* malloc(size_t)
    llvm::IntegerType *i64Ty = llvm::Type::getInt64Ty(context_);
    llvm::PointerType *voidPtr =
        llvm::Type::getInt8Ty(context_)->getPointerTo();
    llvm::FunctionType *mallocFT =
        llvm::FunctionType::get(voidPtr, {i64Ty}, false);
    mallocFn_ = llvm::cast<llvm::Function>(
        module_->getOrInsertFunction("malloc", mallocFT).getCallee());
  }
  if (!freeFn_) {
    // void free(void*)
    llvm::PointerType *voidPtr =
        llvm::Type::getInt8Ty(context_)->getPointerTo();
    llvm::FunctionType *freeFT = llvm::FunctionType::get(
        llvm::Type::getVoidTy(context_), {voidPtr}, false);
    freeFn_ = llvm::cast<llvm::Function>(
        module_->getOrInsertFunction("free", freeFT).getCallee());
  }
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_NEW *node) const noexcept {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_NEW");

  // Asegurarnos de que malloc/free estén declarados
  ensureMallocFreeDeclared();

  // Identificar el nodo de constructor, bien directo o dentro de un AST_CHAINED
  const AST_CONSTRUCTOR_CALL *ctorNode = nullptr;
  if (auto directCtor = dynamic_cast<const AST_CONSTRUCTOR_CALL *>(node->value().get())) {
    // caso 2.a) new A{...} → value() es directamente AST_CONSTRUCTOR_CALL
    ctorNode = directCtor;
  } else if (auto chained = dynamic_cast<const AST_CHAINED *>(node->value().get())) {
    // caso 2.b) new (A{...}.algo) → value() es AST_CHAINED cuyo base() debe ser AST_CONSTRUCTOR_CALL
    auto basePtr = chained->base().get();
    if (auto baseCtor = dynamic_cast<const AST_CONSTRUCTOR_CALL *>(basePtr)) {
      // verificar que no haya otras operaciones encadenadas
      if (!chained->operations().empty()) {
        return createError(ERROR_TYPE::TYPE,
                           "AST_NEW: las operaciones encadenadas tras el constructor no están permitidas");
      }
      ctorNode = baseCtor;
    } else {
      return createError(ERROR_TYPE::TYPE,
                         "AST_NEW: se esperaba que el base de AST_CHAINED fuera un constructor");
    }
  } else {
    return createError(ERROR_TYPE::TYPE,
                       "AST_NEW: se esperaba un AST_CONSTRUCTOR_CALL (directo o en AST_CHAINED)");
  }

  // Ahora ya sabemos que 'ctorNode' apunta al AST_CONSTRUCTOR_CALL correcto.
  // Obtener el tipo LLVM que corresponde al objeto a instanciar:
  auto typeOfNew = ctorNode->returnedFromTypeAnalysis();
  if (!typeOfNew)
    return createError(ERROR_TYPE::TYPE, "type returned is null");
  std::expected<llvm::Type *, Error> llvmTyOrErr =
      typeOfNew->llvmVersion(context_);
  if (!llvmTyOrErr)
    return createError(llvmTyOrErr.error());
  llvm::Type *structTy = *llvmTyOrErr;                       // ej. %A
  llvm::PointerType *structPtrTy = structTy->getPointerTo(); // %A*

  // Calcular tamaño en bytes con DataLayout
  const llvm::DataLayout &DL = module_->getDataLayout();
  uint64_t sizeBytes = DL.getTypeAllocSize(structTy);
  llvm::Value *sizeConst =
      llvm::ConstantInt::get(builder_.getInt64Ty(), sizeBytes);

  // Llamar a malloc(sizeBytes)
  llvm::CallInst *rawPtr =
      builder_.CreateCall(mallocFn_, sizeConst, "new_malloc"); // devuelve i8*

  // Hacer bitcast de i8* a %A*
  llvm::Value *typedPtr =
      builder_.CreateBitCast(rawPtr, structPtrTy,
                             "new_bitcast"); // ahora typedPtr es %A*

  // Generar parámetros del constructor (rvalues)
  llvm::SmallVector<llvm::Value *, 8> callArgs;
  callArgs.push_back(typedPtr); // primer arg: 'this'

  for (auto &argAST : ctorNode->parameters()) {
    auto valOrErr = emitRValue(argAST.get());
    if (!valOrErr)
      return createError(valOrErr.error());
    callArgs.push_back(*valOrErr);
  }

  // Llamar a la función manglada del constructor
  std::string ctorName = "$_ctor_" + ctorNode->id();
  llvm::Function *ctorFn = module_->getFunction(ctorName);
  if (!ctorFn)
    return createError(ERROR_TYPE::FUNCTION, "ctor not found: " + ctorName);

  builder_.CreateCall(ctorFn, callArgs);

  // Actualizar chaining y tipo resultante
  resultChainedExpression_ = typedPtr;
  currentType = typeOfNew; // aunque el ctor sea A, aquí queremos A* en el chaining

  return typedPtr;
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_DELETE *node) const noexcept {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_DELETE");

  // Evaluar el subnodo 'value' en modo rvalue: debe ser un puntero a liberar
  auto ptrOrErr = emitRValue(node->value().get());
  if (!ptrOrErr)
    return createError(ptrOrErr.error());
  llvm::Value *ptrVal = *ptrOrErr; // esto es algo de tipo T*

  // 2) Asegurarnos de que free exista en el módulo
  ensureMallocFreeDeclared();

  // 3) Hacer bitcast a i8* para pasar a free
  llvm::Value *i8Ptr = builder_.CreateBitCast(
      ptrVal, llvm::Type::getInt8Ty(context_)->getPointerTo(),
      "delete_bitcast");

  // 4) Llamar a free(i8*)
  builder_.CreateCall(freeFn_, i8Ptr);

  // delete no retorna valor
  return nullptr;
}


// ---------------------- AST_DEREF ----------------------
std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_DEREF *node) const noexcept {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_DEREF");

  // Obtener rvalue del puntero: debe ser un Value* que apunta a la dirección
  auto ptrOrErr = emitRValue(node->value().get());
  if (!ptrOrErr)
    return createError(ptrOrErr.error());
  llvm::Value *ptrVal = *ptrOrErr;

  // Para un operador *p, devolvemos esa dirección como lvalue en chaining
  resultChainedExpression_ = ptrVal;

  // Ajustar currentType: si p era T*, el resultado de *p debe ser T
  auto ptrType = node->value()->returnedFromTypeAnalysis();
  if (auto pt = std::dynamic_pointer_cast<PointerType>(ptrType)) {
    currentType = pt->baseType();
  } else {
    return createError(ERROR_TYPE::TYPE, "base de deref no es PointerType");
  }

  return ptrVal;
}

} // namespace nicole