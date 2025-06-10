#include "../../../inc/parsingAnalysis/ast/operators/ast_binary.h"
#include "../../../inc/parsingAnalysis/ast/operators/ast_unary.h"
#include "../../../inc/visitors/codeGeneration/codeGeneration.h"
#include <llvm/IR/DerivedTypes.h>
#include <memory>

namespace nicole {

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_BINARY *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_BINARY");
  }

  // Generar subexpresiones
  std::expected<llvm::Value *, Error> leftOrErr =
      emitRValue(node->left().get());
  if (!leftOrErr)
    return createError(leftOrErr.error());
  llvm::Value *L = *leftOrErr;

  std::expected<llvm::Value *, Error> rightOrErr =
      emitRValue(node->right().get());
  if (!rightOrErr)
    return createError(rightOrErr.error());
  llvm::Value *R = *rightOrErr;

  // No uso el tipo que devuelve este nodo porque puede darse el caso de double
  // == double y siempre entre por el case de bool
  std::shared_ptr<Type> semTy = node->left()->returnedFromTypeAnalysis();
  if (std::shared_ptr<ConstType> c =
          std::dynamic_pointer_cast<ConstType>(semTy))
    semTy = c->baseType();

  // Si es BasicType, usar operaciones LLVM correspondientes
  if (std::shared_ptr<BasicType> basic =
          std::dynamic_pointer_cast<BasicType>(semTy)) {
    // return createError(ERROR_TYPE::PRINT_TREE, basic->toString());
    switch (basic->baseKind()) {
    case BasicKind::Int: {
      // Enteros: + - * / % & | comparaciones
      switch (node->op().type()) {
      case TokenType::OPERATOR_ADD:
        return builder_.CreateAdd(L, R, "addtmp");
      case TokenType::OPERATOR_SUB:
        return builder_.CreateSub(L, R, "subtmp");
      case TokenType::OPERATOR_MULT:
        return builder_.CreateMul(L, R, "multmp");
      case TokenType::OPERATOR_DIV:
        return builder_.CreateSDiv(L, R, "divtmp");
      case TokenType::OPERATOR_MODULE:
        return builder_.CreateSRem(L, R, "modtmp");
      case TokenType::AND:
        return builder_.CreateAnd(L, R, "andtmp");
      case TokenType::OR:
        return builder_.CreateOr(L, R, "ortmp");
      case TokenType::EQUAL:
        return builder_.CreateICmpEQ(L, R, "eqtmp");
      case TokenType::NOTEQUAL:
        return builder_.CreateICmpNE(L, R, "netmp");
      case TokenType::OPERATOR_SMALLER:
        return builder_.CreateICmpSLT(L, R, "sltmp");
      case TokenType::SMALLEREQUAL:
        return builder_.CreateICmpSLE(L, R, "sletmp");
      case TokenType::OPERATOR_GREATER:
        return builder_.CreateICmpSGT(L, R, "sgtmp");
      case TokenType::BIGGEREQUAL:
        return builder_.CreateICmpSGE(L, R, "sgetmp");
      default:
        break;
      }
      break;
    }
    case BasicKind::Float:
    case BasicKind::Double: {
      // Flotantes: + - * / comparaciones
      switch (node->op().type()) {
      case TokenType::OPERATOR_ADD:
        return builder_.CreateFAdd(L, R, "faddtmp");
      case TokenType::OPERATOR_SUB:
        return builder_.CreateFSub(L, R, "fsubtmp");
      case TokenType::OPERATOR_MULT:
        return builder_.CreateFMul(L, R, "fmultmp");
      case TokenType::OPERATOR_DIV:
        return builder_.CreateFDiv(L, R, "fdivtmp");
      case TokenType::EQUAL:
        return builder_.CreateFCmpOEQ(L, R, "feqtmp");
      case TokenType::NOTEQUAL:
        return builder_.CreateFCmpUNE(L, R, "fnetmp");
      case TokenType::OPERATOR_SMALLER:
        return builder_.CreateFCmpULT(L, R, "flttmp");
      case TokenType::SMALLEREQUAL:
        return builder_.CreateFCmpULE(L, R, "fletmp");
      case TokenType::OPERATOR_GREATER:
        return builder_.CreateFCmpUGT(L, R, "fgtmp");
      case TokenType::BIGGEREQUAL:
        return builder_.CreateFCmpUGE(L, R, "fgetmp");
      default:
        break;
      }
      break;
    }
    case BasicKind::Bool: {
      // Booleanos: && || == !=
      switch (node->op().type()) {
      case TokenType::AND:
        return builder_.CreateAnd(L, R, "andtmp");
      case TokenType::OR:
        return builder_.CreateOr(L, R, "ortmp");
      case TokenType::EQUAL:
        return builder_.CreateICmpEQ(L, R, "eqtmp");
      case TokenType::NOTEQUAL:
        return builder_.CreateICmpNE(L, R, "netmp");
      default:
        break;
      }
      break;
    }
    case BasicKind::Str: {
      if (node->op().type() == TokenType::OPERATOR_ADD) {
        // Contexto y tipos LLVM
        
        
        llvm::Type *i8Ty = llvm::Type::getInt8Ty(*context_);
        llvm::PointerType *i8Ptr = i8Ty->getPointerTo(); // i8*
        llvm::Type *i64Ty = llvm::Type::getInt64Ty(*context_);

        // Declarar strlen y malloc si no existen
        llvm::FunctionType *strlenFT =
            llvm::FunctionType::get(i64Ty, {i8Ptr}, false);
        llvm::FunctionType *mallocFT =
            llvm::FunctionType::get(i8Ptr, {i64Ty}, false);
        llvm::FunctionCallee strlenFn =
            module_->getOrInsertFunction("strlen", strlenFT);
        llvm::FunctionCallee mallocFn =
            module_->getOrInsertFunction("malloc", mallocFT);

        // Calcular longitudes de L y R
        llvm::Value *lenL = builder_.CreateCall(strlenFn, {L}, "lenL");
        llvm::Value *lenR = builder_.CreateCall(strlenFn, {R}, "lenR");

        // Tamaño = lenL + lenR + 1
        llvm::Value *sumLR = builder_.CreateAdd(lenL, lenR, "sumLR");
        llvm::Value *size = builder_.CreateAdd(
            sumLR, llvm::ConstantInt::get(i64Ty, 1), "allocSize");

        // Llamar a malloc
        llvm::CallInst *buf = builder_.CreateCall(mallocFn, {size}, "buf");

        // Copiar L en buf
        builder_.CreateMemCpy(
            /*Dest=*/buf, llvm::MaybeAlign(),
            /*Src=*/L, llvm::MaybeAlign(),
            /*Size=*/lenL);

        // Copiar R en buf + lenL
        llvm::Value *dstR = builder_.CreateGEP(
            /*PointeeType=*/i8Ty,
            /*Ptr=*/buf,
            /*Index=*/lenL, "dstR");
        builder_.CreateMemCpy(
            /*Dest=*/dstR, llvm::MaybeAlign(),
            /*Src=*/R, llvm::MaybeAlign(),
            /*Size=*/lenR);

        // Escribir '\0' al final: buf[lenL+lenR]
        llvm::Value *sumLR2 = builder_.CreateAdd(lenL, lenR, "sumLR2");
        llvm::Value *termPtr = builder_.CreateGEP(
            /*PointeeType=*/i8Ty,
            /*Ptr=*/buf,
            /*Index=*/sumLR2, "termPtr");
        builder_.CreateStore(llvm::ConstantInt::get(i8Ty, 0), termPtr);

        // devolvemos la posicion en memoria para evitar un doble load en otros
        // sitios
        llvm::AllocaInst *slot =
            builder_.CreateAlloca(i8Ptr, nullptr, "concat_slot");
        builder_.CreateStore(buf, slot);
        return slot;
      }
      switch (node->op().type()) {
      // Igualdad/Desigualdad por puntero
      case TokenType::EQUAL:
        return builder_.CreateICmpEQ(L, R, "ptr_eq");
      case TokenType::NOTEQUAL:
        return builder_.CreateICmpNE(L, R, "ptr_ne");
      case TokenType::OPERATOR_SMALLER:
        return builder_.CreateICmpULT(L, R, "ptr_lt");
      case TokenType::SMALLEREQUAL:
        return builder_.CreateICmpULE(L, R, "ptr_le");
      case TokenType::OPERATOR_GREATER:
        return builder_.CreateICmpUGT(L, R, "ptr_gt");
      case TokenType::BIGGEREQUAL:
        return builder_.CreateICmpUGE(L, R, "ptr_ge");
      default:
        return createError(ERROR_TYPE::TYPE,
                           "operator " + tokenTypeToString(node->op().type()) +
                               " cannot be applied to type Str");
      }
      break;
    }
    default:
      break;
    }
  }

  // Igualdad/desigualdad genérica (punteros, nullptr, enums)
  if (node->op().type() == TokenType::EQUAL ||
      node->op().type() == TokenType::NOTEQUAL) {
    llvm::ICmpInst::Predicate pred = (node->op().type() == TokenType::EQUAL)
                                         ? llvm::ICmpInst::ICMP_EQ
                                         : llvm::ICmpInst::ICMP_NE;
    return builder_.CreateICmp(pred, L, R, "ptreq");
  }

  // Relacionales genéricas (< <= > >=) para punteros o enums
  if (node->op().type() == TokenType::OPERATOR_SMALLER ||
      node->op().type() == TokenType::SMALLEREQUAL ||
      node->op().type() == TokenType::OPERATOR_GREATER ||
      node->op().type() == TokenType::BIGGEREQUAL) {
    llvm::ICmpInst::Predicate pred;
    switch (node->op().type()) {
    case TokenType::OPERATOR_SMALLER:
      pred = llvm::ICmpInst::ICMP_ULT;
      break;
    case TokenType::SMALLEREQUAL:
      pred = llvm::ICmpInst::ICMP_ULE;
      break;
    case TokenType::OPERATOR_GREATER:
      pred = llvm::ICmpInst::ICMP_UGT;
      break;
    case TokenType::BIGGEREQUAL:
      pred = llvm::ICmpInst::ICMP_UGE;
      break;
    default:
      pred = llvm::ICmpInst::ICMP_EQ;
      break;
    }
    return builder_.CreateICmp(pred, L, R, "relctmp");
  }

  return createError(ERROR_TYPE::TYPE,
                     "operator not supported in AST_BINARY: " +
                         tokenTypeToString(node->op().type()));
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_UNARY *node) const noexcept {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_UNARY");

  // Generar código del operando
  std::expected<llvm::Value *, Error> valOrErr =
      emitRValue(node->value().get());
  if (!valOrErr)
    return createError(valOrErr.error());
  llvm::Value *V = *valOrErr;

  // Resolver tipo semántico (desenrollar ConstType)
  std::shared_ptr<Type> semTy = node->returnedFromTypeAnalysis();
  if (std::shared_ptr<ConstType> c =
          std::dynamic_pointer_cast<ConstType>(semTy))
    semTy = c->baseType();

  // Unary sobre BasicType
  if (std::shared_ptr<BasicType> basic =
          std::dynamic_pointer_cast<BasicType>(semTy)) {
    switch (node->op().type()) {
    // Operador lógico NOT: !v
    case TokenType::OPERATOR_NOT:
      // Para bool: bitwise XOR con 1 producirá not lógico
      return builder_.CreateXor(V, llvm::ConstantInt::get(V->getType(), 1),
                                "nottmp");

    // Negación aritmética: -v
    case TokenType::OPERATOR_SUB:
      if (basic->baseKind() == BasicKind::Int) {
        return builder_.CreateNeg(V, "negtmp");
      } else {
        // float o double
        return builder_.CreateFNeg(V, "fnegtmp");
      }

    // Pre-incremento y pre-decremento: ++v, --v
    case TokenType::INCREMENT:
    case TokenType::DECREMENT: {
      bool isInc = (node->op().type() == TokenType::INCREMENT);

      // obtener la dirección de la variable (ptr)
      std::expected<llvm::Value *, Error> ptrOrErr =
          node->value()->accept(*this);
      if (!ptrOrErr)
        return createError(ptrOrErr.error());
      llvm::Value *ptr = *ptrOrErr; // debe ser algo tipo i32*

      // tipo entero LLVM
      
      llvm::Type *i32Ty = llvm::Type::getInt32Ty(*context_);

      // cargar el valor actual
      llvm::Value *oldVal = builder_.CreateLoad(i32Ty, ptr, "ldtmp");

      // crear constante “1”
      llvm::Value *one = llvm::ConstantInt::get(i32Ty, 1);

      // sumar o restar
      llvm::Value *newVal = isInc ? builder_.CreateAdd(oldVal, one, "inctmp")
                                  : builder_.CreateSub(oldVal, one, "dectmp");

      // almacenar el resultado en memoria
      builder_.CreateStore(newVal, ptr);

      // devolver el nuevo valor
      return newVal;
    }

    default:
      break;
    }
  }

  // Unary sobre PointerType
  if (std::shared_ptr<PointerType> ptrType =
          std::dynamic_pointer_cast<PointerType>(semTy)) {
    switch (node->op().type()) {
    case TokenType::OPERATOR_NOT:
      // v == nullptr
      return builder_.CreateICmpEQ(
          V,
          llvm::ConstantPointerNull::get(
              llvm::cast<llvm::PointerType>(V->getType())),
          "isnull");

    case TokenType::INCREMENT:
    case TokenType::DECREMENT: {
      return createError(
          ERROR_TYPE::TYPE,
          "llvm increment and decrement for not implemented yet");
    }

    default:
      break;
    }
  }

  // Resto de casos no soportados
  return createError(ERROR_TYPE::TYPE,
                     "unsupported unary operator: " +
                         tokenTypeToString(node->op().type()));
}

} // namespace nicole