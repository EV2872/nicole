#include "../../../inc/parsingAnalysis/ast/utils/ast_import.h"
#include "../../../inc/parsingAnalysis/ast/utils/ast_print.h"
#include "../../../inc/visitors/codeGeneration/codeGeneration.h"
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <memory>

namespace nicole {

auto printParameters(
    const std::vector<std::pair<llvm::Value *, std::shared_ptr<Type>>> &values,
    llvm::LLVMContext &context, llvm::IRBuilder<> &builder) noexcept
    -> std::vector<std::pair<llvm::Value *, std::string>> {
  std::vector<std::pair<llvm::Value *, std::string>> out;
  out.reserve(values.size());

  // Crear una instancia de i8* para comparación
  llvm::Type *i8Ty = llvm::Type::getInt8Ty(context);
  llvm::Type *i8PtrTy = i8Ty->getPointerTo(/*AddressSpace=*/0);

  for (auto value : values) {
    llvm::Value *origVal{value.first};
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
      if (std::dynamic_pointer_cast<PointerType>(value.second)) {
        fmt = "%p";
      } else {
        fmt = "%s";
      }
    }

    if (fmt.empty())
      llvm::report_fatal_error("Cannot print this type.");

    out.emplace_back(val, fmt);
  }

  return out;
}

auto CodeGeneration::visit(const AST_PRINT *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_PRINT");
  }
  std::vector<std::pair<llvm::Value *, std::shared_ptr<Type>>> values;
  values.reserve(node->values().size());
  for (const std::shared_ptr<AST> &chain : node->values()) {
    std::expected<llvm::Value *, Error> result = emitRValue(chain.get());
    if (!result)
      return createError(result.error());
    llvm::Value *val = *result;
    // VERSION VAR CALL RETORNA ADDR
    values.push_back({val, chain->returnedFromTypeAnalysis()});
  }

  // Llamar a printParameters (sin cambios respecto a tu código)
  std::vector<std::pair<llvm::Value *, std::string>> paramsAndFormats =
      printParameters(values, *context_, builder_);

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
  llvm::Type *i8Ty = llvm::Type::getInt8Ty(*context_);
  llvm::Type *i8PtrTy = i8Ty->getPointerTo(0);
  llvm::FunctionType *printfTy = llvm::FunctionType::get(
      llvm::Type::getInt32Ty(*context_), {i8PtrTy}, /*isVarArg=*/true);
  llvm::FunctionCallee printfFn =
      module_->getOrInsertFunction("printf", printfTy);

  // Crear la cadena de formato global y añadirla como primer argumento
  llvm::Value *fmtStr = builder_.CreateGlobalStringPtr(fullFormatStr, "fmt");
  args.insert(args.begin(), fmtStr);

  // Emitir la llamada a printf y devolverla
  llvm::CallInst *call = builder_.CreateCall(printfFn, args, "calltmp");
  return call;
}

auto CodeGeneration::visit(const AST_IMPORT *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_IMPORT");
  }
  return {};
}

} // namespace nicole