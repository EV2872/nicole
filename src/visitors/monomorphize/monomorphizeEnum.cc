#include "../../../inc/parsingAnalysis/ast/enum/ast_enum.h"
#include "../../../inc/parsingAnalysis/ast/enum/ast_enumAccess.h"
#include "../../../inc/visitors/monomorphize/monomorphize.h"
#include <variant>

namespace nicole {

auto Monomorphize::visit(const AST_ENUM *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ENUM");
  }
  return {};
}

auto Monomorphize::visit(const AST_ENUM_ACCESS *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ENUM_ACCESS");
  }
  return {};
}

} // namespace nicole