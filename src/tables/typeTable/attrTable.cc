#include "../../../inc/tables/typeTable/types/userTypes/attrTable.h"

namespace nicole {

auto AttrTable::has(const std::string &id) const noexcept -> bool {
  return table_.count(id);
}

auto AttrTable::getAttribute(const std::string &id) const noexcept
    -> const std::expected<Attribute, Error> {
  if (table_.count(id)) {
    return table_.at(id);
  }
  return createError(ERROR_TYPE::ATTR, "Attribute: " + id + " does not exist");
}

auto AttrTable::insert(const Attribute &attribute) noexcept
    -> std::expected<std::monostate, Error> {
  if (!has(attribute.id())) {
    table_.emplace(attribute.id(), attribute);
    return {};
  }
  return createError(ERROR_TYPE::ATTR,
                     "the attribute: " + attribute.id() + " already exists");
}

auto AttrTable::setAttribute(const Attribute &attribute) noexcept
    -> std::expected<std::monostate, Error> {
  if (!has(attribute.id())) {
    return createError(ERROR_TYPE::ATTR,
                       "Attribute: " + attribute.id() + " does not exist");
  }
  table_.at(attribute.id()) = attribute;
  return {};
}

} // namespace nicole