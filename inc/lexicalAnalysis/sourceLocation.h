#ifndef SOURCE_LOCATION_H
#define SOURCE_LOCATION_H

#include "token.h"

namespace nicole {

class SourceLocation {
private:
  Token begin_;
  Token end_;

public:
  explicit SourceLocation(const Token &begin, const Token &end) noexcept
      : begin_{begin}, end_{end} {}

  [[nodiscard]] auto beginLoc() const noexcept -> const Token & {
    return begin_;
  }

  [[nodiscard]] auto endLoc() const noexcept -> const Token & { return end_; }

  [[nodiscard]] auto operator<(const SourceLocation &loc) const noexcept
      -> bool {
    if (begin_.location() < loc.begin_.location()) {
      return true;
    }
    if (end_.location() < loc.end_.location()) {
      return true;
    }
    return false;
  }
};

} // namespace nicole

#endif