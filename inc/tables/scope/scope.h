#ifndef SCOPE_H
#define SCOPE_H

#include "../../errors.h"
#include "variable.h"
#include <expected>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace nicole {

class Scope final {
private:
  mutable std::unordered_map<std::string, std::shared_ptr<Variable>> table_;
  mutable std::vector<std::shared_ptr<Variable>> registerForDestruction_{};
  std::shared_ptr<Scope> father_;

public:
  explicit Scope(const std::shared_ptr<Scope> &father) noexcept
      : father_{father} {}

  [[nodiscard]] const std::shared_ptr<Scope> &father() const noexcept {
    return father_;
  }

  [[nodiscard]] bool has(const std::string &id) const noexcept;

  [[nodiscard]] const std::expected<std::shared_ptr<Variable>, Error>
  getVariable(const std::string &id) const noexcept;

  [[nodiscard]] const std::vector<std::shared_ptr<Variable>> &
  registeredForDestruction() const noexcept {
    return registerForDestruction_;
  }

  [[nodiscard]] std::expected<std::monostate, Error>
  insert(const std::shared_ptr<Variable> &variable) noexcept;

  void
  registerForDestruction(const std::shared_ptr<Variable> &variable) noexcept;

  std::expected<std::monostate, Error>
  setVariableType(const std::string &id,
                  const std::shared_ptr<Type> &type) const noexcept;

  friend std::ostream &operator<<(std::ostream &os,
                                  const Scope &scope) noexcept;

}; // namespace nicole

} // namespace nicole

#endif