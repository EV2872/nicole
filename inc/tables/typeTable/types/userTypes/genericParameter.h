#ifndef GENERIC_PARAMETER_H
#define GENERIC_PARAMETER_H

#include <cstddef>
#include <functional>
#include <string>

namespace nicole {

class GenericParameter final {
private:
  std::string name_;

public:
  explicit GenericParameter(const std::string &name) noexcept : name_{name} {}

  [[nodiscard]] auto name() const noexcept -> const std::string & {
    return name_;
  }

  [[nodiscard]] auto operator==(const GenericParameter &other) const noexcept
      -> bool {
    return name_ == other.name_;
  }

  [[nodiscard]] auto operator<(const GenericParameter &other) const noexcept
      -> bool {
    return name_ < other.name_;
  }
};

} // namespace nicole

namespace std {
template <> struct hash<nicole::GenericParameter> {
  auto operator()(const nicole::GenericParameter &obj) const noexcept
      -> std::size_t {
    return std::hash<std::string>{}(obj.name());
  }
};
} // namespace std

#endif