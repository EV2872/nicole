#include "../../../inc/parsingAnalysis/ast/utils/ast_import.h"
#include "../../../inc/parsingAnalysis/ast/utils/ast_print.h"
#include "../../../inc/visitors/codeGeneration/codeGeneration.h"
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <memory>

namespace nicole {

std::vector<std::pair<llvm::Value *, std::string>>
printParameters(const std::vector<llvm::Value *> &values,
                llvm::LLVMContext &context, llvm::IRBuilder<> &builder) {
  std::vector<std::pair<llvm::Value *, std::string>> out;
  out.reserve(values.size());

  // Crear una instancia de i8* para comparación
  llvm::Type *i8Ty = llvm::Type::getInt8Ty(context);
  llvm::Type *i8PtrTy = i8Ty->getPointerTo(/*AddressSpace=*/0);

  for (llvm::Value *origVal : values) {
    if (!origVal)
      llvm::report_fatal_error("Failed to evaluate expression for print.");

    llvm::Value *val = origVal;
    llvm::Type *ty = val->getType();
    std::string fmt;

    if (ty->isIntegerTy(1)) {
      fmt = "%d";
      val = builder.CreateZExt(val, builder.getInt32Ty(), "bool_to_int");
    } else if (ty->isIntegerTy(8)) {
      fmt = "%c";
    } else if (ty->isIntegerTy()) {
      fmt = "%d";
    } else if (ty->isFloatTy()) {
      fmt = "%f";
      val = builder.CreateFPExt(val, llvm::Type::getDoubleTy(context), "fpext");
    } else if (ty->isDoubleTy()) {
      fmt = "%f";
    } else if (ty->isPointerTy() && ty == i8PtrTy) {
      // Cadena C (i8*)
      fmt = "%s";
    }

    if (fmt.empty())
      llvm::report_fatal_error("Cannot print this type.");

    out.emplace_back(val, fmt);
  }

  return out;
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_PRINT *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_PRINT");
  }
  std::vector<llvm::Value *> values;
  values.reserve(node->values().size());
  for (const std::shared_ptr<AST> &chain : node->values()) {
    std::expected<llvm::Value *, Error> result = emitRValue(chain.get());
    if (!result)
      return createError(result.error());
    llvm::Value *val = *result;
    // VERSION VAR CALL RETORNA ADDR
    values.push_back(val);
  }

  // Llamar a printParameters (sin cambios respecto a tu código)
  std::vector<std::pair<llvm::Value *, std::string>> paramsAndFormats =
      printParameters(values, context_, builder_);

  // Concatenar el formato completo y preparar los argumentos
  std::string fullFormatStr;
  fullFormatStr.reserve(paramsAndFormats.size() * 2);
  std::vector<llvm::Value *> args;
  args.reserve(paramsAndFormats.size() + 1);
  for (std::pair<llvm::Value *, std::string> &pf : paramsAndFormats) {
    fullFormatStr += pf.second; // añade "%d", "%f", "%s", …
    args.push_back(pf.first); // añade el llvm::Value* (posiblemente extendido)
  }

  // Obtener/declarar printf con getOrInsertFunction
  llvm::Type *i8Ty = llvm::Type::getInt8Ty(context_);
  llvm::Type *i8PtrTy = i8Ty->getPointerTo(0);
  llvm::FunctionType *printfTy = llvm::FunctionType::get(
      llvm::Type::getInt32Ty(context_), {i8PtrTy}, /*isVarArg=*/true);
  llvm::FunctionCallee printfFn =
      module_->getOrInsertFunction("printf", printfTy);

  // Crear la cadena de formato global y añadirla como primer argumento
  llvm::Value *fmtStr = builder_.CreateGlobalStringPtr(fullFormatStr, "fmt");
  args.insert(args.begin(), fmtStr);

  // Emitir la llamada a printf y devolverla
  llvm::CallInst *call = builder_.CreateCall(printfFn, args, "calltmp");
  return call;
}

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_IMPORT *node) const noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_IMPORT");
  }
  return {};
}

} // namespace nicole