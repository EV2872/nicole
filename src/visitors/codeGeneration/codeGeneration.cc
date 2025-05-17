#include "../../../inc/visitors/codeGeneration/codeGeneration.h"

#include "../../../inc/parsingAnalysis/ast/tree.h"
#include "../../../inc/parsingAnalysis/ast/vector/ast_index.h"
#include <cstdlib>
#include <llvm/Passes/PassBuilder.h>
#include <memory>

/**

FillSemanticInfo ---> insertar delcaraciones en las tablas / insertar tipos /
comprobar que las variables pertenecen al scope (llamadas a variables) /
comrpobar llamadas a enum

TypeAnalysis ---> comprobar en una llamada a funcion que esta existe debido a
sobrecarga de funciones requiere que se trate en el typeAnalysis / igual con
metodos / llamadas a atributos / variables auto

 */

#include <llvm/IR/LegacyPassManager.h> // legacy::PassManager
#include <llvm/MC/TargetRegistry.h>    // TargetRegistry::lookupTarget()
#include <llvm/Support/CodeGen.h>      // CodeGenFileType
#include <llvm/Support/FileSystem.h>   // sys::fs::OF_None
#include <llvm/Support/Program.h>
#include <llvm/Support/TargetSelect.h> // InitializeAllTargets()
#include <llvm/Support/raw_ostream.h>  // errs(), raw_fd_ostream
#include <llvm/Target/TargetMachine.h> // TargetMachine

namespace nicole {

std::expected<std::monostate, Error>
CodeGeneration::emitObjectFile(llvm::Module *module) const noexcept {
  // Inicializar todos los backends y parsers
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();

  // Obtener triple de destino y buscar Target
  auto Triple = LLVMGetDefaultTargetTriple();
  std::string Err;
  const llvm::Target *Target = llvm::TargetRegistry::lookupTarget(Triple, Err);
  if (!Target) {
    llvm::errs() << "Error buscando target: " << Err << "\n";
    return createError(ERROR_TYPE::ENTRY_FILE_NOT_FOUND,
                       "Error buscando target: " + Err);
  }

  // Crear TargetMachine
  llvm::TargetOptions Opts;
  auto TM =
      Target->createTargetMachine(Triple,
                                  "generic", // CPU
                                  "",        // características adicionales
                                  Opts,
                                  llvm::Reloc::PIC_, // Modelo de relocación
                                  llvm::CodeModel::Small, // Modelo de código
                                  llvm::CodeGenOptLevel::Default);

  // Ajustar el módulo
  module->setTargetTriple(Triple);
  module->setDataLayout(TM->createDataLayout());

  // Abrir flujo de salida para el objeto
  std::error_code EC;
  llvm::raw_fd_ostream Dest(options_.binaryName() + ".o", EC,
                            llvm::sys::fs::OF_None);
  if (EC) {
    return createError(ERROR_TYPE::ENTRY_FILE_NOT_FOUND,
                       "Error al abrir " + options_.binaryName() +
                           EC.message());
  }

  // Añadir pases para emitir el objeto y ejecutarlos
  llvm::legacy::PassManager PM;
  if (TM->addPassesToEmitFile(PM, Dest,
                              /*DwoDest=*/nullptr,
                              llvm::CodeGenFileType::ObjectFile)) {
    return createError(ERROR_TYPE::ENTRY_FILE_NOT_FOUND,
                       "Este TargetMachine no puede emitir objeto");
  }
  PM.run(*module);
  Dest.flush();

  return {};
}

std::expected<std::string, Error>
CodeGeneration::nameMangling(const std::shared_ptr<Type> &type) const noexcept {
  std::string mangled{};
  std::expected<std::string, Error> res = nameManglingImpl(type, mangled);
  if (!res)
    return res;
  // quitar posible guión bajo inicial
  if (!mangled.empty() && mangled.front() == '_')
    mangled.erase(mangled.begin());
  return mangled;
}

std::expected<std::string, Error>
CodeGeneration::nameManglingImpl(const std::shared_ptr<Type> &type,
                                 std::string &result) const noexcept {
  if (!type)
    return createError(ERROR_TYPE::TYPE, "null type in name mangling");

  if (std::shared_ptr<BasicType> bt =
          std::dynamic_pointer_cast<BasicType>(type)) {
    result += '_' + bt->toString();
    return result;
  }
  if (std::dynamic_pointer_cast<VoidType>(type)) {
    result += "_void";
    return result;
  }
  if (std::dynamic_pointer_cast<NullType>(type)) {
    result += "_null";
    return result;
  }
  if (std::shared_ptr<PointerType> pt =
          std::dynamic_pointer_cast<PointerType>(type)) {
    std::expected<std::string, Error> rec =
        nameManglingImpl(pt->baseType(), result);
    if (!rec)
      return rec;
    result += "_ptr";
    return result;
  }
  if (std::shared_ptr<VectorType> vt =
          std::dynamic_pointer_cast<VectorType>(type)) {
    std::expected<std::string, Error> rec =
        nameManglingImpl(vt->elementType(), result);
    if (!rec)
      return rec;
    result += "_vec";
    return result;
  }
  if (std::shared_ptr<ConstType> ct =
          std::dynamic_pointer_cast<ConstType>(type)) {
    result += "_const";
    std::expected<std::string, Error> rec =
        nameManglingImpl(ct->baseType(), result);
    if (!rec)
      return rec;
    return result;
  }
  if (std::shared_ptr<EnumType> et =
          std::dynamic_pointer_cast<EnumType>(type)) {
    result += '_' + et->name();
    return result;
  }
  if (std::shared_ptr<GenericInstanceType> git =
          std::dynamic_pointer_cast<GenericInstanceType>(type)) {
    result += '_' + git->name();
    for (const std::shared_ptr<Type> &arg : git->typeArgs()) {
      std::expected<std::string, Error> rec = nameManglingImpl(arg, result);
      if (!rec)
        return rec;
    }
    return result;
  }
  if (std::shared_ptr<UserType> ut =
          std::dynamic_pointer_cast<UserType>(type)) {
    result += '_' + ut->name();
    return result;
  }
  // Fallback
  std::string raw = type->toString();
  for (char c : raw) {
    result += (std::isalnum(static_cast<unsigned char>(c)) ? c : '_');
  }
  return result;
}

std::expected<std::string, Error>
CodeGeneration::nameManglingFunction(const Function &func,
                                     const std::vector<std::shared_ptr<Type>>
                                         &genericReplacements) const noexcept {
  std::string mangled{"$"};
  std::expected<std::string, Error> res =
      nameManglingFunctionImpl(func, genericReplacements, mangled);
  if (!res)
    return res;
  return mangled;
}

std::expected<std::string, Error> CodeGeneration::nameManglingFunctionImpl(
    const Function &func,
    const std::vector<std::shared_ptr<Type>> &genericReplacements,
    std::string &result) const noexcept {
  result += '_';
  result += func.id();

  for (const std::shared_ptr<Type> &genType : genericReplacements) {
    std::expected<std::string, Error> resType = nameMangling(genType);
    if (!resType)
      return createError(resType.error());
    result += '_';
    result += *resType;
  }

  for (const std::pair<std::string, std::shared_ptr<Type>> &param :
       func.params().params()) {
    std::expected<std::string, Error> resParam = nameMangling(param.second);
    if (!resParam)
      return createError(resParam.error());
    result += '_';
    result += *resParam;
  }

  std::expected<std::string, Error> resRet = nameMangling(func.returnType());
  if (!resRet)
    return createError(resRet.error());
  result += "_ret_";
  result += *resRet;

  return result;
}

bool isVectorElement(const AST *node) noexcept {
  return dynamic_cast<const AST_INDEX *>(node) != nullptr;
}

// EMIT LVALUE: simplemente delega al visitor y devuelve la dirección
std::expected<llvm::Value *, Error>
CodeGeneration::emitLValue(const AST *node) const noexcept {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST for lvalue");
  // asumimos que el visit de ese nodo produce *siempre* un
  // AllocaInst*/dirección
  return node->accept(*this);
}

// EMIT RVALUE: carga una vez la dirección devuelta por emitLValue
std::expected<llvm::Value *, Error>
CodeGeneration::emitRValue(const AST *node) const noexcept {
  std::expected<llvm::Value *, Error> valOrErr = emitLValue(node);
  if (!valOrErr)
    return createError(valOrErr.error());
  llvm::Value *val = *valOrErr;

  std::expected<llvm::Type *, Error> tyOrErr =
      node->returnedFromTypeAnalysis()->llvmVersion(context_);
  if (!tyOrErr)
    return createError(tyOrErr.error());
  llvm::Type *llvmTy = *tyOrErr;

  // Agregados completos…
  if (llvmTy->isAggregateType()) {
    llvm::AllocaInst *tmp = builder_.CreateAlloca(llvmTy, nullptr, "agg_tmp");
    const llvm::DataLayout &DL = module_->getDataLayout();
    builder_.CreateMemCpy(tmp, llvm::MaybeAlign(), val, llvm::MaybeAlign(),
                          DL.getTypeAllocSize(llvmTy));
    return tmp;
  }

  // Si es puntero:
  if (val->getType()->isPointerTy()) {
    // sólo cargamos si NO es una constante
    if (!llvm::isa<llvm::Constant>(val) && !isVectorElement(node)) {
      // esto cubre variables locales (alloca), argumentos, etc.
      return builder_.CreateLoad(llvmTy, val, "rval_load");
    }
    // en cambio, literales de cadena (CreateGlobalStringPtr → Constant*)
    // se dejan tal cual, como i8* apuntando al .rodata
    return val;
  }

  // escalar ya cargado (int, float, bool…)
  return val;
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_STATEMENT *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_STATEMENT");
  }
  return node->expression()->accept(*this);
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_BODY *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_BODY");
  }
  auto parentScope = currentScope_;
  currentScope_ = node->scope();

  llvm::Value *lastValue{nullptr};

  // Recorremos todas las sentencias del cuerpo
  for (const std::shared_ptr<AST_STATEMENT> &statement : node->body()) {
    std::expected<llvm::Value *, Error> res = statement->accept(*this);
    if (!res) {
      // Propagamos el error si alguna sentencia falla
      return createError(res.error());
    }
    if (builder_.GetInsertBlock()->getTerminator())
      break;
    // Guardamos el valor devuelto (puede ser nullptr si la sentencia no produce
    // valor)
    lastValue = res.value();
  }
  currentScope_ = parentScope;

  // Devolvemos el valor de la última sentencia (o nullptr si no había
  // sentencias)
  return lastValue;
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const Tree *tree) const noexcept {
  if (!tree) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid Tree");
  }

  if (options_.validateTree()) {
    llvm::Type *i32Ty = llvm::Type::getInt32Ty(context_);
    llvm::FunctionType *wrapFT =
        llvm::FunctionType::get(i32Ty, /*isVarArg=*/false);
    mainFunction_ = llvm::Function::Create(
        wrapFT, llvm::Function::ExternalLinkage, "main", module_.get());

    // Insertamos el bloque de entrada ANTES de generar el cuerpo de usuario
    entry_ = llvm::BasicBlock::Create(context_, "entry", mainFunction_);
    builder_.SetInsertPoint(entry_);

    // Ahora generamos TODO el AST de usuario (incluido $_main) ---
    if (std::expected<llvm::Value *, Error> res = tree->root()->accept(*this);
        !res)
      return createError(res.error());

    // Llamamos a la función del usuario ($_main) y retornamos su i32 ---
    llvm::Function *userMain = module_->getFunction("$_main");
    if (!userMain)
      return createError(ERROR_TYPE::FUNCTION, "no existe $_main");

    llvm::Value *ret = builder_.CreateCall(userMain, {}, "call_user_main");
    builder_.CreateRet(ret);
  } else {
    std::expected<llvm::Type *, Error> mainType{typeTable_->intType()->llvmVersion(context_)};
    /*if (const std::shared_ptr<NoPropagateType> isNopropagateType{
            std::dynamic_pointer_cast<NoPropagateType>(
                tree->root()->returnedFromTypeAnalysis())}) {
      mainType = typeTable_->voidType()->llvmVersion(context_);
    } else {
      mainType =
          tree->root()->returnedFromTypeAnalysis()->llvmVersion(context_);
      if (!mainType) {
        return createError(mainType.error());
      }
    }*/
    if (!mainType) {
        return createError(mainType.error());
      }

    funcType_ = llvm::FunctionType::get(*mainType, false);
    mainFunction_ = llvm::Function::Create(
        funcType_, llvm::Function::ExternalLinkage, "main", module_.get());
    entry_ = llvm::BasicBlock::Create(context_, "entry", mainFunction_);
    builder_.SetInsertPoint(entry_);

    const std::expected<llvm::Value *, Error> result{
        tree->root()->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
    if (!builder_.GetInsertBlock()->getTerminator())
      builder_.CreateRet(llvm::ConstantInt::get(
          *typeTable_->intType()->llvmVersion(context_), 0));
  }

  if (options_.optimize()) {
    llvm::PassBuilder PB;
    llvm::LoopAnalysisManager LAM;
    llvm::FunctionAnalysisManager FAM;
    llvm::CGSCCAnalysisManager CGAM;
    llvm::ModuleAnalysisManager MAM;

    PB.registerModuleAnalyses(MAM);
    PB.registerCGSCCAnalyses(CGAM);
    PB.registerFunctionAnalyses(FAM);
    PB.registerLoopAnalyses(LAM);
    PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

    llvm::ModulePassManager MPM =
        PB.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O3, true);
    MPM.run(*module_, MAM);
  }

  llvm::verifyModule(*module_, &llvm::errs());

  if (options_.printIR()) {
    module_->print(llvm::outs(), nullptr);
  }

  if (options_.justInTime()) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    std::string errStr;
    llvm::ExecutionEngine *execEngine =
        llvm::EngineBuilder(std::move(module_))
            .setErrorStr(&errStr)
            .setOptLevel(llvm::CodeGenOptLevel::Default)
            .create();
    if (!execEngine) {
      std::cerr << "Failed to create ExecutionEngine: " << errStr << std::endl;
      return nullptr;
    }

    // Ejecuta la función main y obtiene el resultado
    std::vector<llvm::GenericValue> noargs;
    llvm::GenericValue gv = execEngine->runFunction(mainFunction_, noargs);

    delete execEngine;

    return nullptr;
  }

  auto emited{emitObjectFile(module_.get())};

  if (!emited) {
    return createError(emited.error());
  }

  system(std::string{"clang++ " + options_.binaryName() + ".o -o " +
                     options_.binaryName()}
             .c_str());

  return nullptr;
}

} // namespace nicole