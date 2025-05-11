#include "../../../inc/parsingAnalysis/ast/userTypes/ast_attrAccess.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_constructorCall.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_methodCall.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_struct.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_this.h"
#include "../../../inc/visitors/codeGeneration/codeGeneration.h"

namespace nicole {

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_STRUCT *node) const noexcept {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_STRUCT");
  insideStruct = true;
  auto foundType{typeTable_->getType(node->id())};
  if (!foundType) {
    return createError(foundType.error());
  }
  if (const auto isUserType{std::dynamic_pointer_cast<UserType>(*foundType)}) {
    currentUserType_ = isUserType;
  }
  // Recuperar el UserType y su LLVM StructType (con cuerpo ya definido por
  // llvmVersion)
  auto ut = typeTable_->getType(node->id());
  if (!ut) {
    return createError(ut.error());
  }
  std::expected<llvm::Type *, Error> stOrErr = (*ut)->llvmVersion(context_);
  if (!stOrErr)
    return createError(stOrErr.error());
  // No necesitamos un Value* aquí: la declaración de struct solo define el tipo

  // Generar el IR para el constructor (si existe)
  if (node->constructor()) {
    auto ctorOrErr = node->constructor()->accept(*this);
    if (!ctorOrErr)
      return createError(ctorOrErr.error());
  }

  // Generar el IR para el destructor (si existe)
  if (node->destructor()) {
    auto dtorOrErr = node->destructor()->accept(*this);
    if (!dtorOrErr)
      return createError(dtorOrErr.error());
  }

  // Generar el IR para cada método
  for (const auto &method : node->methods()) {
    auto mOrErr = method->accept(*this);
    if (!mOrErr)
      return createError(mOrErr.error());
  }
  insideStruct = false;
  // La declaración de un struct no produce un llvm::Value*, así que
  // devolvemos nullptr
  return nullptr;
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_ATTR_ACCESS *node) const noexcept {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ATTR_ACCESS");

  // La dirección del struct sobre el que accedemos está en
  // resultChainedExpression_
  llvm::Value *basePtr = resultChainedExpression_;
  if (!basePtr)
    return createError(ERROR_TYPE::TYPE,
                       "no base pointer for attribute access");

  // Recuperar el UserType de currentType
  auto userStTy = std::dynamic_pointer_cast<UserType>(currentType);
  if (!userStTy)
    return createError(ERROR_TYPE::TYPE, "base is not a user‐defined struct");

  // Obtener el LLVM StructType desde el UserType
  auto stOrErr = userStTy->llvmVersion(context_);
  if (!stOrErr)
    return createError(stOrErr.error());
  auto *llvmStTy = llvm::cast<llvm::StructType>(*stOrErr);

  // Calcular el índice del campo a partir del nombre
  unsigned fieldIdx;
  {
    auto attrOrErr = userStTy->getAttribute(node->id());
    if (!attrOrErr)
      return createError(attrOrErr.error());
    fieldIdx = static_cast<unsigned int>(attrOrErr->position());
  }

  // Generar el GEP al campo
  llvm::Value *fieldPtr =
      builder_.CreateStructGEP(llvmStTy, basePtr, fieldIdx, node->id());

  // Actualizar estado para el chaining
  resultChainedExpression_ = fieldPtr;
  // y el nuevo tipo es el tipo de ese atributo
  currentType = userStTy->getAttribute(node->id())->type();

  return fieldPtr;
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_METHOD_CALL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_METHOD_CALL");
  }
  for (const std::shared_ptr<AST> &arg : node->parameters()) {
    const std::expected<llvm::Value *, Error> result{arg->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return {};
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_METHOD_DECL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_METHOD_DECL");
  }
  const std::expected<llvm::Value *, Error> result{node->body()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return {};
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_CONSTRUCTOR_DECL *node) const noexcept {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_CONSTRUCTOR_DECL");
  std::shared_ptr<Scope> parentScope = currentScope_;
  currentScope_ = node->body()->scope();
  // Obtener el type de retorno (debe ser void)
  llvm::Type *retTy = llvm::Type::getVoidTy(context_);

  // Colectar los tipos de los parámetros (incluyendo 'this' como primero)
  llvm::SmallVector<llvm::Type *, 8> paramTys;
  // 'this' apunta al struct
  auto userStTy = std::dynamic_pointer_cast<UserType>(node->returnType());
  if (!userStTy)
    return createError(ERROR_TYPE::TYPE, "constructor no retorna UserType");
  std::expected<llvm::Type *, Error> structOrErr =
      userStTy->llvmVersion(context_);
  if (!structOrErr)
    return createError(structOrErr.error());
  // usar puntero al struct, no struct por valor
  paramTys.push_back(llvm::PointerType::getUnqual(*structOrErr)); // this*

  for (auto &pr : node->parameters().params()) {
    std::expected<llvm::Type *, Error> tyOrErr =
        pr.second->llvmVersion(context_);
    if (!tyOrErr)
      return createError(tyOrErr.error());
    paramTys.push_back(*tyOrErr);
  }

  // Crear el FunctionType y la Function en el módulo
  llvm::FunctionType *fnTy = llvm::FunctionType::get(retTy, paramTys, false);
  std::string fnName = "$_ctor_" + node->id();
  llvm::Function *fn = llvm::cast<llvm::Function>(
      module_->getOrInsertFunction(fnName, fnTy).getCallee());
  fn->setLinkage(llvm::GlobalValue::ExternalLinkage);

  // Crear el bloque entry y preparar builder
  llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(context_, "entry", fn);
  llvm::IRBuilder<>::InsertPointGuard guard(builder_);
  builder_.SetInsertPoint(entryBB);

  // Allocas para los parámetros y registro en scope
  unsigned idx = 0;
  for (auto &arg : fn->args()) {
    llvm::AllocaInst *slot = builder_.CreateAlloca(
        arg.getType(), nullptr,
        (idx == 0 ? "this_ptr" : ("arg" + std::to_string(idx))));
    builder_.CreateStore(&arg, slot);
    if (idx == 0) {
      auto thisVar = std::make_shared<Variable>(
          std::string("this"),
          node->returnType(), // el UserType de la struct
          /* value */ &arg);

      thisVar->setAddress(slot);
      auto err = currentScope_->insert(thisVar);
      if (!err)
        return createError(err.error());
    } else {
      const auto paramName = node->parameters().params()[idx - 1].first;
      auto varOrErr = currentScope_->getVariable(paramName);
      if (!varOrErr)
        return createError(varOrErr.error());
      auto varPtr = *varOrErr;
      // ■ Asignarle el AllocaInst* para que tenga dirección
      varPtr->setAddress(slot);
    }
    ++idx;
  }

  bool prevInsideStruct = insideStruct;
  auto prevUserType = currentUserType_;

  // Generar cuerpo del constructor
  if (auto bodyOrErr = node->body()->accept(*this); !bodyOrErr)
    return createError(bodyOrErr.error());

  // Restaurar flags de struct
  insideStruct = prevInsideStruct;
  currentUserType_ = prevUserType;

  // Emitir return
  if (!builder_.GetInsertBlock()->getTerminator())
    builder_.CreateRetVoid();
  currentScope_ = parentScope;

  return fn;
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_SUPER *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_SUPER");
  }
  for (const std::shared_ptr<AST> &arg : node->arguments()) {
    const std::expected<llvm::Value *, Error> result{arg->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return {};
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_DESTRUCTOR_DECL *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_DESTRUCTOR_DECL");
  }
  const std::expected<llvm::Value *, Error> result{node->body()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return {};
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_THIS *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_THIS");
  }
  return {};
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_CONSTRUCTOR_CALL *node) const noexcept {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_CONSTRUCTOR_CALL");

  // Reservar espacio para el objeto
  auto tyOrErr = node->returnedFromTypeAnalysis()->llvmVersion(context_);
  if (!tyOrErr)
    return createError(tyOrErr.error());
  llvm::AllocaInst *objAlloca =
      builder_.CreateAlloca(*tyOrErr, nullptr, node->id() + "_ptr");

  // Empaquetar this + parámetros
  llvm::SmallVector<llvm::Value *, 8> callArgs;
  callArgs.push_back(objAlloca);
  for (auto &argAST : node->parameters()) {
    auto valOrErr = emitRValue(argAST.get());
    if (!valOrErr)
      return createError(valOrErr.error());
    callArgs.push_back(*valOrErr);
  }

  // Llamar al constructor manglado
  std::string ctorName = "$_ctor_" + node->id();
  llvm::Function *ctorFn = module_->getFunction(ctorName);
  if (!ctorFn)
    return createError(ERROR_TYPE::FUNCTION, "ctor not found: " + ctorName);
  builder_.CreateCall(ctorFn, callArgs, node->id() + "_ctor");

  // Actualizar estado y devolver puntero al objeto
  resultChainedExpression_ = objAlloca;
  currentType = node->returnedFromTypeAnalysis();
  return objAlloca;
}
} // namespace nicole