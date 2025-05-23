#ifndef CATEGORY_H
#define CATEGORY_H

#include <iostream>
#include <string>
#include <vector>

#include "regexWrapper.h"
#include "token.h"

namespace nicole {

// Represents de category of a token
class Category final {
 private:
  TokenType type_{};
  RegexWrapper pattern_;
  bool skip_{false};

 public:
  Category(const TokenType& type, const std::string& matcher, const bool skip)
      : type_{type}, pattern_{matcher}, skip_{skip} {};

  TokenType type() const { return type_; }

  RegexWrapper pattern() const { return pattern_; }

  std::string rawPattern() const { return pattern_.str(); }

  bool skip() const { return skip_; } // to know if it must be skipped like comments

  bool matchToken(const Token& token) const {
    return regex_match(token.raw(), pattern_.pattern());
  };
};

}  // namespace nicole

#endif