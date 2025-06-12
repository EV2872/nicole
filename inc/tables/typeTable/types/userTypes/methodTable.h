#ifndef METHOD_TABLE_H
#define METHOD_TABLE_H

#include "../../../../errors.h"
#include "genericParameter.h"
#include "method.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace nicole {

class MethodTable {
private:
  mutable std::unordered_map<std::string, std::vector<Method>> table_;

public:
  [[nodiscard]] auto getMethods(const std::string &id) const noexcept
      -> std::vector<Method>;

  auto insert(const Method &method) noexcept -> void;

  auto print() const noexcept -> void;

  [[nodiscard]] auto
  setMethodReturnType(const std::string &id,
                      const std::shared_ptr<Type> &type) noexcept
      -> std::expected<std::monostate, Error>;

  [[nodiscard]] auto setMethodParameters(const std::string &id,
                                         const Parameters &params) noexcept
      -> std::expected<std::monostate, Error>;

  [[nodiscard]] auto
  setMethodGenercis(const std::string &id,
                    const std::vector<GenericParameter> &generics) noexcept
      -> std::expected<std::monostate, Error>;
};

} // namespace nicole

#endif