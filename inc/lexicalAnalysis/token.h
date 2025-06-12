#ifndef TOKEN_H
#define TOKEN_H

#include "location.h"
#include "type.h"
#include <string>

namespace nicole {

// Used to simplify the parsing analysis
class Token final {
private:
  TokenType type_;
  std::string raw_;
  Location loc_;

public:
  explicit Token(const TokenType &type, const std::string &raw,
                 const Location &loc) noexcept
      : type_{type}, raw_{raw}, loc_{loc} {};

  [[nodiscard]] auto type() const noexcept -> TokenType { return type_; };

  [[nodiscard]] auto raw() const noexcept -> const std::string & {
    return raw_;
  }

  [[nodiscard]] auto location() const noexcept -> const Location & {
    return loc_;
  }

  [[nodiscard]] auto locInfo() const noexcept -> const std::string {
    return loc_.file().string() + " " + std::to_string(loc_.row()) + ':' +
           std::to_string(loc_.col());
  }
};

} // namespace nicole

#endif