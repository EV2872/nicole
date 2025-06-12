#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include "../../../tables/typeTable/types/type.h"
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace nicole {

class Attributes {
private:
  std::vector<std::pair<std::string, std::shared_ptr<Type>>> params_;

public:
  explicit Attributes(
      const std::vector<std::pair<std::string, std::shared_ptr<Type>>>
          &params) noexcept
      : params_{params} {}

  [[nodiscard]] auto params() const noexcept
      -> const std::vector<std::pair<std::string, std::shared_ptr<Type>>> & {
    return params_;
  }

  [[nodiscard]] auto size() const noexcept -> std::size_t {
    return params_.size();
  }

  auto begin() const noexcept -> decltype(params_.begin()) {
    return params_.begin();
  }

  auto end() const noexcept -> decltype(params_.end()) { return params_.end(); }
};

} // namespace nicole

#endif