#ifndef FUNCTION_TABLE_H
#define FUNCTION_TABLE_H

#include "../../errors.h"
#include "function.h"
#include <expected>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace nicole {

class FunctionTable {
private:
  std::unordered_map<std::string, std::vector<Function>> table_;

public:
  [[nodiscard]] auto getFunctions(const std::string &id) const noexcept
      -> std::vector<Function>;

  auto insert(const Function &function) noexcept -> void;

  auto print() const noexcept -> void;

  [[nodiscard]] auto
  setFuncReturnType(const std::string &id,
                    const std::shared_ptr<Type> &type) noexcept
      -> std::expected<std::monostate, Error>;

  [[nodiscard]] auto setFuncParameters(const std::string &id,
                                       const Parameters &params) noexcept
      -> std::expected<std::monostate, Error>;

  [[nodiscard]] auto nameMangling(const Function &type) const noexcept
      -> std::expected<std::string, Error>;

  [[nodiscard]] auto nameManglingImpl(const Function &type,
                                      std::string &result) const noexcept
      -> std::expected<std::string, Error>;
};

} // namespace nicole

#endif