#include "../../../inc/tables/functionTable/functionTable.h"
#include <iostream>

namespace nicole {

auto FunctionTable::getFunctions(const std::string &id) const noexcept
    -> std::vector<Function> {
  if (table_.count(id)) {
    return table_.at(id);
  }
  return {};
}

auto FunctionTable::insert(const Function &function) noexcept -> void {
  table_[function.id()].push_back(function);
}

auto FunctionTable::print() const noexcept -> void {
  std::cout << "Functions:\n";
  for (const std::pair<const std::string, std::vector<nicole::Function>>
           &functions : table_) {
    for (const Function &function : functions.second) {
      std::cout << function.id() << "\n";
    }
  }
}

auto FunctionTable::setFuncReturnType(
    const std::string &id, const std::shared_ptr<Type> &type) noexcept
    -> std::expected<std::monostate, Error> {
  // Comprobar que existe alguna función con ese id.
  if (table_.find(id) == table_.end() || table_.at(id).empty()) {
    return createError(ERROR_TYPE::FUNCTION, "Function not found: " + id);
  }
  // Actualizamos la última función insertada con ese id.
  table_[id].back().setReturnType(type);
  return std::expected<std::monostate, Error>{std::monostate{}};
}

auto FunctionTable::setFuncParameters(const std::string &id,
                                      const Parameters &type) noexcept
    -> std::expected<std::monostate, Error> {
  // Comprobar que existe alguna función con ese id.
  if (table_.find(id) == table_.end() || table_.at(id).empty()) {
    return createError(ERROR_TYPE::FUNCTION, "Function not found: " + id);
  }
  // Actualizamos los parámetros de la última función insertada con ese id.
  // Se asume que el setter de parámetros acepta un std::shared_ptr<Type>.
  table_[id].back().setParameters(type);
  return std::expected<std::monostate, Error>{std::monostate{}};
}

auto FunctionTable::nameMangling(const Function &type) const noexcept
    -> std::expected<std::string, Error> {
  std::string result{"$"};
  return nameManglingImpl(type, result);
}

auto FunctionTable::nameManglingImpl(const Function &type,
                                     std::string &result) const noexcept
    -> std::expected<std::string, Error> {
  if (type.returnType()) {
  }
  return result;
}

} // namespace nicole