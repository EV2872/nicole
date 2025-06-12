#include "../../../inc/parsingAnalysis/ast/enum/ast_enum.h"
#include "../../../inc/parsingAnalysis/ast/enum/ast_enumAccess.h"
#include "../../../inc/visitors/fillSemanticInfo/fillSemanticInfo.h"
#include <memory>
#include <set>
#include <variant>

namespace nicole {

auto FillSemanticInfo::visit(const AST_ENUM *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ENUM");
  }
  std::set<std::string> noRepetitions{node->identifiers().begin(),
                                      node->identifiers().end()};
  if (noRepetitions.size() != node->identifiers().size()) {
    return createError(ERROR_TYPE::ENUM,
                       "a enum cannot have two identifiers with the same name");
  }
  const auto insert{typeTable_->insert(
      std::make_shared<EnumType>(node->id(), node->identifiers()))};
  if (!insert) {
    return createError(insert.error());
  }
  return {};
}

auto FillSemanticInfo::visit(const AST_ENUM_ACCESS *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ENUM_ACCESS");
  }
  const auto typeExists{typeTable_->getType(node->id())};
  if (!typeExists) {
    return createError(typeExists.error());
  }
  const auto enumType{std::dynamic_pointer_cast<EnumType>(*typeExists)};
  if (!enumType) {
    return createError(
        ERROR_TYPE::TYPE,
        "attempting to access a type as a enum but it isn't enum");
  }
  if (!enumType->hasIdentifier(node->identifier())) {
    return createError(ERROR_TYPE::TYPE, "the enum " + enumType->name() +
                                             " does not have the identifier " +
                                             node->identifier());
  }
  return {};
}

} // namespace nicole