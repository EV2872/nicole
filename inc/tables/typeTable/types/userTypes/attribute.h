#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "../../../symbol.h"
#include "../type.h"
#include <cstddef>
#include <memory>

namespace nicole {

class Attribute final : public Symbol {
private:
  std::shared_ptr<Type> type_{};
  mutable std::size_t position_;

public:
  explicit Attribute(const std::string &id, const std::shared_ptr<Type> &type,
                     const std::size_t pos) noexcept
      : Symbol{id}, type_{type}, position_(pos) {}

  [[nodiscard]] auto type() const noexcept -> const std::shared_ptr<Type> & {
    return type_;
  }

  [[nodiscard]] auto position() const noexcept -> std::size_t {
    return position_;
  }

  auto setPosition(const std::size_t pos) const noexcept -> void {
    position_ = pos;
  }
};

} // namespace nicole

#endif