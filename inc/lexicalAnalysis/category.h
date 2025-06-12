#ifndef CATEGORY_H
#define CATEGORY_H

#include <string>

#include "regexWrapper.h"
#include "token.h"
#include "type.h"

namespace nicole {

// Represents de category of a token
class Category final {
private:
  TokenType type_;
  RegexWrapper pattern_;
  bool skip_;

public:
  explicit Category(const TokenType &type, const std::string &matcher,
                    const bool skip) noexcept
      : type_{type}, pattern_{matcher}, skip_{skip} {};

  [[nodiscard]] auto type() const noexcept -> TokenType { return type_; }

  [[nodiscard]] auto pattern() const noexcept -> const RegexWrapper & {
    return pattern_;
  }

  [[nodiscard]] auto rawPattern() const noexcept -> const std::string & {
    return pattern_.str();
  }

  // to know if it must be skipped like comments
  [[nodiscard]] auto skip() const noexcept -> bool { return skip_; }

  [[nodiscard]] auto matchToken(const Token &token) const noexcept -> bool {
    return pattern_.match(token.raw());
  };
};

} // namespace nicole

#endif