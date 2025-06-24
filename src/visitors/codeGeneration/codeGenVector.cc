#include "../../../inc/visitors/codeGeneration/codeGeneration.h"

#include "../../../inc/parsingAnalysis/ast/vector/ast_index.h"
#include "../../../inc/parsingAnalysis/ast/vector/ast_vector.h"

#include <cstddef>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Value.h>
#include <memory>

namespace nicole {

auto CodeGeneration::visit(const AST_VECTOR *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_VECTOR");

  // Recuperar el VectorType y su StructType { T* data; i64 size; }
  std::shared_ptr<VectorType> vecType =
      std::dynamic_pointer_cast<VectorType>(node->returnedFromTypeAnalysis());
  if (!vecType)
    return createError(ERROR_TYPE::TYPE, "AST_VECTOR no retorna VectorType");
  std::expected<llvm::Type *, Error> structOrErr =
      vecType->llvmVersion(*context_);
  if (!structOrErr)
    return createError(structOrErr.error());
  llvm::StructType *vecStruct = llvm::cast<llvm::StructType>(*structOrErr);

  // Tipo del elemento y número de entradas
  std::expected<llvm::Type *, Error> elemTyOrErr =
      vecType->elementType()->llvmVersion(*context_);
  if (!elemTyOrErr)
    return createError(elemTyOrErr.error());
  llvm::Type *elemTy = *elemTyOrErr;

  size_t count = node->values().size();
  llvm::Value *numElems =
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), count);

  // Crear allocas en el bloque entry()
  llvm::AllocaInst *vecAlloca, *dataAlloca;
  {
    // Mover el IRBuilder al entry block para las allocas
    llvm::IRBuilder<> entryBuilder(entry_);
    entryBuilder.SetInsertPoint(entry_, entry_->getFirstInsertionPt());

    // Alocar el struct vector
    vecAlloca = entryBuilder.CreateAlloca(vecStruct, nullptr, "vec_literal");

    // Alocar buffer de elementos T[numElems]
    dataAlloca = entryBuilder.CreateAlloca(elemTy, numElems, "vec_data");
  }

  // Para quien venga encadenando, el resultado de vector es el struct ptr
  resultChainedExpression_ = vecAlloca;

  // Rellenar el buffer, elemento a elemento
  for (size_t i = 0; i < count; ++i) {
    // Generar el valor del elemento (puede ser ptr a struct o valor escalar)
    std::expected<llvm::Value *, Error> valOrErr =
        emitRValue(node->values()[i].get());
    if (!valOrErr)
      return createError(valOrErr.error());
    llvm::Value *elemValPtr = *valOrErr;

    // Obtener puntero al slot i: &dataAlloca[i]
    llvm::Value *idx = builder_.getInt64(i);
    llvm::Value *slotPtr =
        builder_.CreateInBoundsGEP(elemTy,     // tipo de elemento
                                   dataAlloca, // puntero al primer elemento
                                   idx,        // índice
                                   "vec_elem_gep");

    // Si T es un aggregate (struct, otro vector...), cargar y store profundo
    if (elemTy->isAggregateType()) {
      llvm::Value *loaded = builder_.CreateLoad(elemTy, elemValPtr, "agg_load");
      builder_.CreateStore(loaded, slotPtr);
    } else {
      // Escalar o puntero: store directo
      builder_.CreateStore(elemValPtr, slotPtr);
    }
  }

  // Escribir campos .data y .size en el struct
  // vecAlloca->data = dataAlloca
  llvm::Value *dataField =
      builder_.CreateStructGEP(vecStruct, vecAlloca, 0, "vec_data_ptr");
  builder_.CreateStore(dataAlloca, dataField);

  // vecAlloca->size = numElems
  llvm::Value *sizeField =
      builder_.CreateStructGEP(vecStruct, vecAlloca, 1, "vec_size_ptr");
  builder_.CreateStore(numElems, sizeField);

  // Actualizar tipo y devolver el ptr al struct
  currentType = vecType;
  return vecAlloca;
}

auto CodeGeneration::visit(const AST_INDEX *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_INDEX");

  // como el indice puede venir deducido de un chained guardamos los tipos
  // previos
  auto prevType{currentType};
  auto prevResultchained = resultChainedExpression_;

  std::expected<llvm::Value *, Error> idxOrErr =
      emitRValue(node->index().get());
  if (!idxOrErr)
    return createError(idxOrErr.error());
  llvm::Value *idxVal = *idxOrErr;
  if (idxVal->getType()->isIntegerTy(32)) {
    idxVal = builder_.CreateSExt(idxVal, llvm::Type::getInt64Ty(*context_),
                                 "idx_to_i64");
  }

  // Dirección base (lvalue) de la expresión encadenada
  llvm::Value *basePtr = prevResultchained;
  if (!basePtr || !basePtr->getType()->isPointerTy())
    return createError(ERROR_TYPE::TYPE, "base of index is not a pointer");

  // Si es vector, extraer el puntero al buffer (campo .data)
  if (std::shared_ptr<VectorType> vecType =
          std::dynamic_pointer_cast<VectorType>(prevType)) {
    // Tipo del struct del vector
    std::expected<llvm::Type *, Error> structOrErr =
        vecType->llvmVersion(*context_);
    if (!structOrErr)
      return createError(structOrErr.error());
    llvm::StructType *vecStructTy = llvm::cast<llvm::StructType>(*structOrErr);

    // Cargar el struct completo
    llvm::Value *vecVal =
        builder_.CreateLoad(vecStructTy, basePtr, "load_vec_struct");
    // Extraer .data (índice 0)
    llvm::Value *dataPtr =
        builder_.CreateExtractValue(vecVal, {0}, "vec_data_ptr");

    // Calcular GEP **sobre elemTy**, no sobre el struct
    std::expected<llvm::Type *, Error> elemTyOrErr =
        vecType->elementType()->llvmVersion(*context_);
    if (!elemTyOrErr)
      return createError(elemTyOrErr.error());
    llvm::Type *elemTy = *elemTyOrErr;

    // dataPtr es puntero a elemTy (por ejemplo i8* para Str),
    // así que su tipo es elemTy*
    llvm::Value *elemPtr =
        builder_.CreateInBoundsGEP(elemTy,  // tipo de elemento
                                   dataPtr, // puntero al primer elemento
                                   idxVal,  // índice
                                   "vec_elem_gep");

    resultChainedExpression_ = elemPtr;
    currentType = vecType->elementType();
    return elemPtr;
  }

  // Si es cadena, igual: devolvemos la dirección al carácter
  if (std::shared_ptr<BasicType> basic =
          std::dynamic_pointer_cast<BasicType>(prevType);
      basic && basic->baseKind() == BasicKind::Str) {
    // cargar el i8* de la variable
    llvm::Value *strPtr =
        builder_.CreateLoad(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0),
                            basePtr, "load_str_ptr");

    llvm::Value *charPtr = builder_.CreateInBoundsGEP(
        builder_.getInt8Ty(), strPtr, idxVal, "str_elem_gep");
    resultChainedExpression_ = charPtr;
    currentType = std::make_shared<BasicType>(BasicKind::Char);
    return charPtr;
  }

  return createError(ERROR_TYPE::TYPE,
                     "indexed type is neither vector nor string");
}

} // namespace nicole