#include "../../../inc/parsingAnalysis/ast/utils/ast_import.h"
#include "../../../inc/parsingAnalysis/ast/utils/ast_print.h"
#include "../../../inc/visitors/monomorphize/monomorphize.h"
#include <variant>

namespace nicole {

auto Monomorphize::visit(const AST_PRINT *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_PRINT");
  }
  for (const auto &chain : node->values()) {
    const auto result{chain->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  return {};
}

auto Monomorphize::visit(const AST_IMPORT *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_IMPORT");
  }
  return {};
}

} // namespace nicole