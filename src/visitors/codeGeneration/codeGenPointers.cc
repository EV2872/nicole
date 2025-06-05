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

  // 1) Asegurarnos de que malloc/free estén declarados en el módulo
  ensureMallocFreeDeclared();

  // 2) Intentamos ver si node->value() es un AST_CONSTRUCTOR_CALL (tipo usuario)
  const AST_CONSTRUCTOR_CALL *ctorNode = 
      dynamic_cast<const AST_CONSTRUCTOR_CALL *>(node->value().get());

  // 3) Si esConstructor → manejamos tipo usuario con constructor
  if (ctorNode) {
    // ———————— CAMINO PARA TIPOS USUARIO ————————

    // 3.1) Obtener el tipo LLVM de “ctorNode”
    auto typeOfNew = ctorNode->returnedFromTypeAnalysis();
    if (!typeOfNew)
      return createError(ERROR_TYPE::TYPE, "type returned is null");
    std::expected<llvm::Type *, Error> llvmTyOrErr =
        typeOfNew->llvmVersion(context_);
    if (!llvmTyOrErr)
      return createError(llvmTyOrErr.error());
    llvm::Type *structTy = *llvmTyOrErr;                       // ej. %A
    llvm::PointerType *structPtrTy = structTy->getPointerTo(); // %A*

    // 3.2) Calcular tamaño en bytes
    const llvm::DataLayout &DL = module_->getDataLayout();
    uint64_t sizeBytes = DL.getTypeAllocSize(structTy);
    llvm::Value *sizeConst =
        llvm::ConstantInt::get(builder_.getInt64Ty(), sizeBytes);

    // 3.3) Llamar a malloc(sizeBytes) → devuelve i8*
    llvm::CallInst *rawPtr =
        builder_.CreateCall(mallocFn_, sizeConst, "new_malloc"); // i8*

    // 3.4) Hacer bitcast de i8* a %A*
    llvm::Value *typedPtr =
        builder_.CreateBitCast(rawPtr,
                               structPtrTy,
                               "new_bitcast"); // ahora typedPtr es %A*

    // 3.5) Generar parámetros del constructor (todos rvalues)
    llvm::SmallVector<llvm::Value *, 8> callArgs;
    callArgs.push_back(typedPtr); // primer arg: “this”

    for (auto &argAST : ctorNode->parameters()) {
      auto valOrErr = emitRValue(argAST.get());
      if (!valOrErr)
        return createError(valOrErr.error());
      callArgs.push_back(*valOrErr);
    }

    // 3.6) Llamar a la función manglada del constructor
    std::string ctorName = "$_ctor_" + ctorNode->id();
    llvm::Function *ctorFn = module_->getFunction(ctorName);
    if (!ctorFn)
      return createError(ERROR_TYPE::FUNCTION, "ctor not found: " + ctorName);

    builder_.CreateCall(ctorFn, callArgs);

    // 3.7) Actualizar chaining y tipo resultante para expresiones encadenadas
    resultChainedExpression_ = typedPtr;
    currentType = typeOfNew; // aunque el ctor sea A, aquí queremos A* en el chaining

    return typedPtr;
  }

  // 4) ———————— CAMINO PARA TIPOS BÁSICOS (int, double, i8*, etc.) ————————
  // node->value() NO es AST_CONSTRUCTOR_CALL.
  // Por ejemplo puede ser:
  //   new 5
  //   new 3.14
  //   new "hola"
  //   new &someVar       (si en tu gramática permites new sobre una expresión puntero)
  //
  // 4.1) Recuperar el tipo devuelto por TypeAnalysis de node->value()
  auto typeOfNew = node->value()->returnedFromTypeAnalysis();
  if (!typeOfNew)
    return createError(ERROR_TYPE::TYPE, "type returned is null");
  std::expected<llvm::Type *, Error> llvmTyOrErr =
    typeOfNew->llvmVersion(context_);
  if (!llvmTyOrErr)
    return createError(llvmTyOrErr.error());
  llvm::Type *allocTy = *llvmTyOrErr;               // ej. i32, double, i8*, etc.
  llvm::PointerType *allocPtrTy = allocTy->getPointerTo(); // ej. i32*, double*, i8**,...

  // 4.2) Calcular tamaño en bytes
  const llvm::DataLayout &DL = module_->getDataLayout();
  uint64_t sizeBytes = DL.getTypeAllocSize(allocTy);
  llvm::Value *sizeConst =
      llvm::ConstantInt::get(builder_.getInt64Ty(), sizeBytes);

  // 4.3) Llamar a malloc(sizeBytes)
  llvm::CallInst *rawPtr =
      builder_.CreateCall(mallocFn_, sizeConst, "new_malloc"); // i8*

  // 4.4) Hacer bitcast de i8* a T* (en este momento T = allocTy)
  llvm::Value *typedPtr =
      builder_.CreateBitCast(rawPtr,
                             allocPtrTy,
                             "new_bitcast_basic"); // ahora typedPtr es T*

  // 4.5) Evaluar la expresión “node->value()” para obtener su rvalue
  //      (por ejemplo si node->value() era '5', esto crea un ConstantInt 5;
  //       si era "hola", es un i8* al literal; si era 3.14, un ConstantFP, etc.)
  auto valOrErr = emitRValue(node->value().get());
  if (!valOrErr)
    return createError(valOrErr.error());
  llvm::Value *initialVal = *valOrErr; // esto ya es un Value* del tipo allocTy o i8*

  // 4.6) Si el tipo es agregado (por ejemplo un struct, o std::string implícito),
  //      habría que hacer MemCpy. Pero aquí estamos en “básicos”: i32, double, i8*…
  //      Así que simplemente hacemos un store simple:
  builder_.CreateStore(initialVal, typedPtr);

  // 4.7) Preparar chaining y tipo resultante
  resultChainedExpression_ = typedPtr;
  currentType = typeOfNew; // para new 5, currentType = int; pero en chaining devuelve int*?

  return typedPtr;
}


std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_DELETE *node) const noexcept {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_DELETE");

  // Obtener l-value (la dirección de la variable) para luegi asignarle null
  auto addrOrErr = emitLValue(node->value().get());
  if (!addrOrErr)
    return createError(addrOrErr.error());
  llvm::Value *varAddr = *addrOrErr;  

  // Evaluar el subnodo 'value' en modo rvalue: debe ser un puntero a liberar
  auto ptrOrErr = emitRValue(node->value().get());
  if (!ptrOrErr)
    return createError(ptrOrErr.error());
  llvm::Value *ptrVal = *ptrOrErr; // esto es algo de tipo T*

  // Asegurarnos de que free exista en el módulo
  ensureMallocFreeDeclared();

  // Hacer bitcast a i8* para pasar a free
  llvm::Value *i8Ptr = builder_.CreateBitCast(
      ptrVal, llvm::Type::getInt8Ty(context_)->getPointerTo(),
      "delete_bitcast");

  // Llamar a free(i8*)
  builder_.CreateCall(freeFn_, i8Ptr);

  // Ahora guardar un null del mismo tipo en la variable
  llvm::PointerType *ptrTy = 
      llvm::cast<llvm::PointerType>(ptrVal->getType()); // T*
  llvm::Constant *nullConst = llvm::Constant::getNullValue(ptrTy);
  builder_.CreateStore(nullConst, varAddr);

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