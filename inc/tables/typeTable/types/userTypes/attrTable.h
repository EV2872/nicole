#ifndef ATTR_TABLE_H
#define ATTR_TABLE_H

#include "../../../../errors.h"
#include "attribute.h"
#include <cstddef>
#include <expected>
#include <string>
#include <unordered_map>

namespace nicole {

class AttrTable final {
private:
  mutable std::unordered_map<std::string, Attribute> table_;

public:
  [[nodiscard]] auto has(const std::string &id) const noexcept -> bool;

  [[nodiscard]] auto size() const noexcept -> std::size_t {
    return table_.size();
  }

  [[nodiscard]] auto getAttribute(const std::string &id) const noexcept
      -> const std::expected<Attribute, Error>;

  [[nodiscard]] auto insert(const Attribute &variable) noexcept
      -> std::expected<std::monostate, Error>;

  [[nodiscard]] auto setAttribute(const Attribute &variable) noexcept
      -> std::expected<std::monostate, Error>;

  [[nodiscard]] auto begin() const noexcept -> decltype(table_.begin()) {
    return table_.begin();
  }

  [[nodiscard]] auto end() const noexcept -> decltype(table_.end()) {
    return table_.end();
  }
}; // namespace nicole

} // namespace nicole

#endif