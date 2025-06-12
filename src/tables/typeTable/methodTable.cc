#include "../../../inc/tables/typeTable/types/userTypes/methodTable.h"
#include <iostream>

namespace nicole {

auto MethodTable::getMethods(const std::string &id) const noexcept
    -> std::vector<Method> {
  if (table_.count(id)) {
    return table_.at(id);
  }
  return {};
}

auto MethodTable::insert(const Method &method) noexcept -> void {
  table_[method.id()].push_back(method);
}

auto MethodTable::print() const noexcept -> void {
  std::cout << "Methods:\n";
  for (const std::pair<const std::string, std::vector<nicole::Method>>
           &methods : table_) {
    for (const Method &method : methods.second) {
      std::cout << method.id() << "\n";
    }
  }
}

auto MethodTable::setMethodReturnType(
    const std::string &id, const std::shared_ptr<Type> &type) noexcept
    -> std::expected<std::monostate, Error> {
  if (table_.find(id) == table_.end() || table_.at(id).empty()) {
    return createError(ERROR_TYPE::METHOD, "Method not found: " + id);
  }
  // Se actualiza la última entrada del método con el nuevo tipo de retorno.
  table_[id].back().setReturnType(type);
  return std::expected<std::monostate, Error>{std::monostate{}};
}

auto MethodTable::setMethodParameters(const std::string &id,
                                      const Parameters &params) noexcept
    -> std::expected<std::monostate, Error> {
  if (table_.find(id) == table_.end() || table_.at(id).empty()) {
    return createError(ERROR_TYPE::METHOD, "Method not found: " + id);
  }
  // Se actualizan los parámetros del método.
  table_[id].back().setParameters(params);
  return std::expected<std::monostate, Error>{std::monostate{}};
}

auto MethodTable::setMethodGenercis(
    const std::string &id, const std::vector<GenericParameter> &params) noexcept
    -> std::expected<std::monostate, Error> {
  if (table_.find(id) == table_.end() || table_.at(id).empty()) {
    return createError(ERROR_TYPE::METHOD, "Method not found: " + id);
  }
  // Se actualiza la lista de genéricos del método.
  table_[id].back().setGenerics(params);
  return std::expected<std::monostate, Error>{std::monostate{}};
}

} // namespace nicole