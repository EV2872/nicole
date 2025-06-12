#ifndef SINTAX_H
#define SINTAX_H

#include "lexer.h"
#include <string>

namespace nicole {

// Abstract class that represents the sintax of a language
class Sintax {
private:
  std::string extension_;

public:
  explicit Sintax(const std::string &extension) noexcept
      : extension_{extension} {}

  virtual ~Sintax() = default;

  [[nodiscard]] auto extension() const noexcept -> const std::string & {
    return extension_;
  }

  [[nodiscard]] virtual auto createLexer() const noexcept -> Lexer = 0;
};

} // namespace nicole

#endif