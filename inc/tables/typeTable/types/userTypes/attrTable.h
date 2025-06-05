#ifndef ATTR_TABLE_H
#define ATTR_TABLE_H

#include "../../../../errors.h"
#include "attribute.h"
#include <expected>
#include <string>
#include <unordered_map>

namespace nicole {

class AttrTable final {
private:
  mutable std::unordered_map<std::string, Attribute> table_;

public:
  [[nodiscard]] bool has(const std::string &id) const noexcept;

  [[nodiscard]] size_t size() const noexcept { return table_.size(); }

  [[nodiscard]] const std::expected<Attribute, Error>
  getAttribute(const std::string &id) const noexcept;

  [[nodiscard]] std::expected<std::monostate, Error>
  insert(const Attribute &variable) noexcept;

  [[nodiscard]] std::expected<std::monostate, Error>
  setAttribute(const Attribute &variable) noexcept;

  [[nodiscard]] auto begin() const noexcept { return table_.begin(); }

  [[nodiscard]] auto end() const noexcept { return table_.end(); }
}; // namespace nicole

} // namespace nicole

#endif