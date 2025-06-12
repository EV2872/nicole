#include "../../../inc/tables/scope/scope.h"

namespace nicole {

auto Scope::has(const std::string &id) const noexcept -> bool {
  if (table_.count(id)) {
    return true;
  }
  if (father_) {
    return father_->has(id);
  }
  return false;
}

auto Scope::getVariable(const std::string &id) const noexcept
    -> const std::expected<std::shared_ptr<Variable>, Error> {
  if (table_.count(id)) {
    return table_.at(id);
  }
  if (father_) {
    return father_->getVariable(id);
  }
  return createError(ERROR_TYPE::VARIABLE,
                     "variable: " + id + " does not exist");
}

auto Scope::insert(const std::shared_ptr<Variable> &variable) noexcept
    -> std::expected<std::monostate, Error> {
  if (!has(variable->id())) {
    table_.emplace(variable->id(), variable);
    return {};
  }
  return createError(ERROR_TYPE::VARIABLE,
                     "the variable: " + variable->id() + " already exists");
}

auto Scope::registerForDestruction(
    const std::shared_ptr<Variable> &variable) noexcept -> void {
  registerForDestruction_.push_back(variable);
}

auto Scope::setVariableType(const std::string &id,
                            const std::shared_ptr<Type> &type) const noexcept
    -> std::expected<std::monostate, Error> {
  auto it = table_.find(id);
  if (it != table_.end()) {
    it->second->setType(type);
    return {};
  }
  if (father_) {
    return father_->setVariableType(id, type);
  }
  return createError(ERROR_TYPE::VARIABLE,
                     "variable '" + id + "' does not exist");
}

auto operator<<(std::ostream &os, const Scope &scope) noexcept
    -> std::ostream & {
  os << "Scope { ";
  for (auto it = scope.table_.cbegin(); it != scope.table_.cend(); ++it) {
    os << it->first;
    if (std::next(it) != scope.table_.cend()) {
      os << ", ";
    }
  }
  os << " }";
  if (scope.father_) {
    os << " -> " << *scope.father_;
  }
  return os;
}

} // namespace nicole