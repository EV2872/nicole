#include "../../../inc/parsingAnalysis/ast/functions/ast_funcCall.h"
#include "../../../inc/parsingAnalysis/ast/functions/ast_funcDecl.h"
#include "../../../inc/parsingAnalysis/ast/functions/ast_return.h"
#include "../../../inc/visitors/codeGeneration/codeGeneration.h"

#include <cstddef>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Value.h>
#include <memory>

namespace nicole {

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_FUNC_CALL *node) const noexcept {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_FUNC_CALL");
  // 1) Evaluar argumentos
  llvm::SmallVector<llvm::Value *, 8> argValues;
  argValues.reserve(node->parameters().size());
  for (const std::shared_ptr<AST> &expr : node->parameters()) {
    std::expected<llvm::Value *, Error> valOrErr = emitRValue(expr.get());
    if (!valOrErr)
      return createError(valOrErr.error());
    argValues.push_back(*valOrErr);
  }

  // Obtener la declaración monomorfizada elegida
  const AST_FUNC_DECL *decl = node->declReference();
  auto &formalParams = decl->parameters().params();

  if (!decl)
    return createError(ERROR_TYPE::FUNCTION,
                       "no declaration bound for call to " + node->id());
  bool alreadyMangled{false};
  if (node->replaceOfGenerics().size()) {
    alreadyMangled = true;
    std::expected<llvm::Value *, Error> monoFunction{visit(decl)};
    if (!monoFunction) {
      return createError(monoFunction.error());
    }
    std::cout << "codeGen " << node->id() << " " << node->nodeId() << "\n";
    std::cout << "~~~~~~~~~~~~~" << decl->id() << "\n";
    std::cout << "~~~~~~~~~~~~~" << decl->generics().size() << "\n";
  }

  // Construir el descriptor Function a partir de la decl
  Function funcDesc{decl->id(), decl->generics(),
                    decl->parameters(), // usa Parameters para firma
                    decl->returnType(), decl->body()};

  // Mangling del nombre de la función
  std::expected<std::string, Error> mNameOrErr =
      (alreadyMangled) ? decl->id() : nameManglingFunctionDecl(funcDesc);
  if (!mNameOrErr)
    return createError(mNameOrErr.error());
  std::string fnName = *mNameOrErr;

  // Recuperar el llvm::Function* del módulo
  llvm::Function *callee = module_->getFunction(fnName);
  if (!callee)
    return createError(ERROR_TYPE::FUNCTION,
                       "LLVM function not found: " + fnName);

  // Ajustar y empaquetar argumentos
  llvm::SmallVector<llvm::Value *, 8> callArgs;
  llvm::FunctionType *ft = callee->getFunctionType();
  for (size_t i = 0; i < argValues.size(); ++i) {
    llvm::Value *v = argValues[i];
    auto &formalTy = formalParams[i].second;

    if (auto basic = std::dynamic_pointer_cast<BasicType>(formalTy)) {
      if (basic->baseKind() == BasicKind::Str && v->getType()->isPointerTy() &&
          llvm::isa<llvm::Constant>(v)) { // literal global
        v = builder_.CreateCall(strdupFn_, {v}, "strdup_call");
      }
    }

    llvm::Type *paramTy = ft->getParamType(static_cast<unsigned int>(i));

    // Si el parámetro es un struct (agregado) y v es un puntero, cargamos:
    if (paramTy->isAggregateType() && v->getType()->isPointerTy()) {
      v = builder_.CreateLoad(paramTy, v, node->id() + "_arg_load");
    }

    if (v->getType() != paramTy) {
      if (v->getType()->isIntegerTy() && paramTy->isIntegerTy())
        v = builder_.CreateSExt(v, paramTy, "arg_sext");
      else
        v = builder_.CreateBitCast(v, paramTy, "arg_cast");
    }
    callArgs.push_back(v);
  }

  // Emitir la llamada y actualizar estado de chaining
  llvm::CallInst *ci =
      builder_.CreateCall(callee, callArgs, node->id() + "_call");
  resultChainedExpression_ = ci;
  currentType = decl->returnType();

  return ci;
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_FUNC_DECL *node) const noexcept {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_FUNC_DECL");
  if (node->generics().size()) {
    return nullptr;
  }
  if (!options_.validateTree())
    isMain = false;
  std::shared_ptr<Scope> parentScope = currentScope_;
  currentScope_ = node->body()->scope();
  // Calcular nombre mangled de la declaración (sin genéricos de llamada)
  std::expected<std::string, Error> mNameOrErr = nameManglingFunctionDecl(
      Function{node->id(), node->generics(), node->parameters(),
               node->returnType(), nullptr});
  if (!mNameOrErr)
    return createError(mNameOrErr.error());
  std::string fnName = *mNameOrErr;

  // Crear o recuperar llvm::Function
  llvm::SmallVector<llvm::Type *, 8> paramTys;
  for (const std::pair<std::string, std::shared_ptr<Type>> &p :
       node->parameters().params()) {
    std::expected<llvm::Type *, Error> tyOrErr =
        p.second->llvmVersion(context_);
    if (!tyOrErr)
      return createError(tyOrErr.error());
    paramTys.push_back(*tyOrErr);
  }
  std::expected<llvm::Type *, Error> retTyOrErr =
      node->returnType()->llvmVersion(context_);
  if (!retTyOrErr)
    return createError(retTyOrErr.error());
  llvm::FunctionType *fnTy =
      llvm::FunctionType::get(*retTyOrErr, paramTys, false);

  llvm::Function *fn = llvm::cast<llvm::Function>(
      module_->getOrInsertFunction(fnName, fnTy).getCallee());
  fn->setLinkage(llvm::GlobalValue::ExternalLinkage);

  // Reservar slots para parámetros y registrarlos
  llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(context_, "entry", fn);
  entry_ = entryBB;
  llvm::IRBuilder<> entryBuilder(entryBB);
  unsigned idx = 0;
  for (const std::pair<std::string, std::shared_ptr<Type>> &pr :
       node->parameters().params()) {
    llvm::Argument &arg = *std::next(fn->arg_begin(), idx);
    arg.setName(pr.first);

    // Alloca + store
    llvm::AllocaInst *slot =
        entryBuilder.CreateAlloca(arg.getType(), nullptr, pr.first + ".addr");
    entryBuilder.CreateStore(&arg, slot);

    // Variable registra slot y valor inicial
    // auto varPtr = std::make_shared<Variable>(pr.first, pr.second, nullptr);
    std::expected<std::shared_ptr<Variable>, Error> varOrErr{
        currentScope_->getVariable(pr.first)};
    if (!varOrErr) {
      return createError(varOrErr.error());
    }
    std::shared_ptr<Variable> varPtr = *varOrErr; // shared_ptr<Variable>
    Variable &var = *varPtr;                      // referencia al objeto real
    var.setAddress(slot);
    var.setValue(&arg);

    ++idx;
  }
  // Generar el cuerpo
  llvm::IRBuilder<>::InsertPointGuard guard(builder_);
  builder_.SetInsertPoint(entryBB);
  if (std::expected<llvm::Value *, Error> bodyErr = node->body()->accept(*this);
      !bodyErr)
    return createError(bodyErr.error());
  if (fn->getReturnType()->isVoidTy() && !entryBB->getTerminator())
    builder_.CreateRetVoid();
  if (!options_.validateTree())
    isMain = true;
  currentScope_ = parentScope;
  return fn;
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_RETURN *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_RETURN");
  }
  if (!node->expression() and isMain) {
    return builder_.CreateRet(llvm::ConstantInt::get(
        *typeTable_->intType()->llvmVersion(context_), 0));
  }
  if (!node->expression()) {
    return builder_.CreateRetVoid();
  }
  const std::expected<llvm::Value *, Error> result{
      emitRValue(node->expression().get())};
  if (!result) {
    return createError(result.error());
  }
  return builder_.CreateRet(*result);
}

std::expected<std::string, Error>
CodeGeneration::nameManglingFunctionDecl(const Function &func) const noexcept {
  if (options_.validateTree() and func.id() == "main") {
    return "$_main";
  }
  // Prefijo
  std::string mangled = "$";

  // Nombre base de la función
  mangled += "_";
  mangled += func.id(); // id() es el nombre base sin parámetros

  // Genéricos (si los hay)
  for (const GenericParameter &genType : func.generics()) {
    mangled += "_";
    mangled += genType.name();
  }

  // Parámetros formales
  for (const std::pair<std::string, std::shared_ptr<Type>> &param :
       func.params().params()) {
    std::string tmp;
    if (std::expected<std::string, Error> res =
            nameManglingImpl(param.second, tmp);
        !res)
      return createError(res.error());
    mangled += "_";
    mangled += tmp;
  }

  // Tipo de retorno
  {
    std::string tmp;
    if (std::expected<std::string, Error> res =
            nameManglingImpl(func.returnType(), tmp);
        !res)
      return createError(res.error());
    mangled += "_ret_";
    mangled += tmp;
  }

  // Limpiar posible guión bajo inicial
  if (!mangled.empty() && mangled.front() == '_')
    mangled.erase(mangled.begin());

  return mangled;
}

} // namespace nicole