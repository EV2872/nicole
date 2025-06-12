#ifndef REGEX_NC_H
#define REGEX_NC_H

#include <regex>
#include <string>

namespace nicole {

// Regex doesn´t store a raw string of the pattern so a wrapper is need like the
// one used in boost library
class RegexWrapper final {
private:
  /* data */
  std::string rawPattern_;
  std::regex pattern_;

public:
  explicit RegexWrapper(const std::string &rawPattern) noexcept
      : rawPattern_{rawPattern},
        pattern_{rawPattern, std::regex_constants::optimize} {};

  [[nodiscard]] auto str() const noexcept -> const std::string & {
    return rawPattern_;
  }

  [[nodiscard]] auto pattern() const noexcept -> const std::regex & {
    return pattern_;
  }

  [[nodiscard]] auto match(const std::string &str) const noexcept -> bool {
    return std::regex_match(str, pattern_);
  }
};

} // namespace nicole

#endif