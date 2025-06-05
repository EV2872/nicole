#include "../../../inc/tables/scope/scope.h"

namespace nicole {

bool Scope::has(const std::string &id) const noexcept {
  if (table_.count(id)) {
    return true;
  }
  if (father_) {
    return father_->has(id);
  }
  return false;
}

const std::expected<std::shared_ptr<Variable>, Error>
Scope::getVariable(const std::string &id) const noexcept {
  if (table_.count(id)) {
    return table_.at(id);
  }
  if (father_) {
    return father_->getVariable(id);
  }
  return createError(ERROR_TYPE::VARIABLE,
                     "variable: " + id + " does not exist");
}

std::expected<std::monostate, Error>
Scope::insert(const std::shared_ptr<Variable> &variable) noexcept {
  if (!has(variable->id())) {
    table_.emplace(variable->id(), variable);
    return {};
  }
  return createError(ERROR_TYPE::VARIABLE,
                     "the variable: " + variable->id() + " already exists");
}

void Scope::registerForDestruction(
    const std::shared_ptr<Variable> &variable) noexcept {
  registerForDestruction_.push_back(variable);
}

std::expected<std::monostate, Error>
Scope::setVariableType(const std::string &id,
                       const std::shared_ptr<Type> &type) const noexcept {
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

std::ostream &operator<<(std::ostream &os, const Scope &scope) noexcept {
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