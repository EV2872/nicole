#include "../../../inc/parsingAnalysis/ast/literals/ast_bool.h"
#include "../../../inc/parsingAnalysis/ast/literals/ast_char.h"
#include "../../../inc/parsingAnalysis/ast/literals/ast_double.h"
#include "../../../inc/parsingAnalysis/ast/literals/ast_float.h"
#include "../../../inc/parsingAnalysis/ast/literals/ast_int.h"
#include "../../../inc/parsingAnalysis/ast/literals/ast_null.h"
#include "../../../inc/parsingAnalysis/ast/literals/ast_string.h"
#include "../../../inc/visitors/codeGeneration/codeGeneration.h"
#include <cstdint>

namespace nicole {

auto CodeGeneration::visit(const AST_BOOL *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_BOOL");
  }
  return llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_),
                                node->value());
}

auto CodeGeneration::visit(const AST_CHAR *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_CHAR");
  }
  return llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context_),
                                static_cast<uint64_t>(node->value()));
}

auto CodeGeneration::visit(const AST_DOUBLE *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_DOUBLE");
  }
  return llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_),
                               llvm::APFloat(node->value()));
}

auto CodeGeneration::visit(const AST_FLOAT *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_FLOAT");
  }
  return llvm::ConstantFP::get(llvm::Type::getFloatTy(*context_),
                               llvm::APFloat(node->value()));
}

auto CodeGeneration::visit(const AST_INT *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_INT");
  }
  return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_),
                                static_cast<uint64_t>(node->value()), true);
}

auto CodeGeneration::visit(const AST_NULL *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_NULL");
  }
  llvm::Type *i8Ty = llvm::Type::getInt8Ty(*context_);
  llvm::PointerType *i8PtrTy = llvm::PointerType::get(i8Ty, /*AddressSpace=*/0);
  // Devolver un null de ese tipo
  return llvm::ConstantPointerNull::get(i8PtrTy);
}

auto CodeGeneration::visit(const AST_STRING *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_STRING");
  }
  llvm::Value *strPtr =
      builder_.CreateGlobalString(node->value(), // contenido de la cadena
                                     "str"); // nombre simbólico en el módulo
  return strPtr;
}

} // namespace nicole