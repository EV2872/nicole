#include "../../../inc/visitors/codeGeneration/codeGeneration.h"

#include "../../../inc/parsingAnalysis/ast/chained/ast_chained.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_attrAccess.h"
#include "../../../inc/parsingAnalysis/ast/userTypes/ast_methodCall.h"
#include "../../../inc/parsingAnalysis/ast/vector/ast_index.h"

#include <memory>

namespace nicole {

std::expected<llvm::Value *, Error>
CodeGeneration::visit(const AST_CHAINED *node) const noexcept {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_CHAINED");

  // VAR_CALL → emitLValue, FUNC_CALL/CTOR/ENUM → emitRValue
  llvm::Value *current;
  switch (node->base()->type()) {
  case AST_TYPE::VAR_CALL: {
    std::expected<llvm::Value *, Error> addr = emitLValue(node->base().get());
    if (!addr)
      return createError(addr.error());
    current = *addr;
    break;
  }
  case AST_TYPE::FUN_CALL:
  case AST_TYPE::CONSTRUCTOR_CALL:
  case AST_TYPE::ENUM_ACCESS: {
    std::expected<llvm::Value *, Error> val = emitRValue(node->base().get());
    if (!val)
      return createError(val.error());
    current = *val;
    break;
  }
  default:
    return createError(ERROR_TYPE::TYPE,
                       "unsupported base in chained expression");
  }
  resultChainedExpression_ = current;
  currentType = node->base()->returnedFromTypeAnalysis();

  // Recorre las operaciones encadenadas
  for (const std::shared_ptr<AST> &op : node->operations()) {
    llvm::Value *next;
    if (const AST_ATTR_ACCESS *attr =
            dynamic_cast<const AST_ATTR_ACCESS *>(op.get())) {
      // acceso a atributo → lvalue
      std::expected<llvm::Value *, Error> addr = emitLValue(attr);
      if (!addr)
        return createError(addr.error());
      next = *addr;
    } else if (const AST_INDEX *idx =
                   dynamic_cast<const AST_INDEX *>(op.get())) {
      // acceso por índice rvalue, devuelve el valor dentro del vector/str
      std::expected<llvm::Value *, Error> val = emitRValue(idx);
      if (!val)
        return createError(val.error());
      next = *val;
    } else if (const AST_METHOD_CALL *mcall =
                   dynamic_cast<const AST_METHOD_CALL *>(op.get())) {
      // llamada a método → rvalue
      std::expected<llvm::Value *, Error> val = emitRValue(mcall);
      if (!val)
        return createError(val.error());
      next = *val;
    } else {
      return createError(ERROR_TYPE::TYPE,
                         "unsupported operation in chained expression");
    }

    // Actualiza para la siguiente iteración
    resultChainedExpression_ = next;
    currentType = op->returnedFromTypeAnalysis();
  }
  return resultChainedExpression_;
}

} // namespace nicole