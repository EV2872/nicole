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
  std::weak_ptr<Scope> father_;

public:
  explicit Scope(const std::shared_ptr<Scope> &father) noexcept
      : father_{father} {}

  [[nodiscard]] auto father() const noexcept -> const std::shared_ptr<Scope> {
    return father_.lock();
  }

  [[nodiscard]] auto has(const std::string &id) const noexcept -> bool;

  [[nodiscard]] auto getVariable(const std::string &id) const noexcept
      -> const std::expected<std::shared_ptr<Variable>, Error>;

  [[nodiscard]] auto registeredForDestruction() const noexcept
      -> const std::vector<std::shared_ptr<Variable>> & {
    return registerForDestruction_;
  }

  [[nodiscard]] auto insert(const std::shared_ptr<Variable> &variable) noexcept
      -> std::expected<std::monostate, Error>;

  auto
  registerForDestruction(const std::shared_ptr<Variable> &variable) noexcept
      -> void;

  auto setVariableType(const std::string &id,
                       const std::shared_ptr<Type> &type) const noexcept
      -> std::expected<std::monostate, Error>;

  friend auto operator<<(std::ostream &os, const Scope &scope) noexcept
      -> std::ostream &;

}; // namespace nicole

} // namespace nicole

#endif