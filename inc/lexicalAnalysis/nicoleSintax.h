#ifndef NICOLE_SINTAX_H
#define NICOLE_SINTAX_H

#include "sintax.h"

namespace nicole {

// The sintax used for the Nicole language
class NicoleSintax final : public Sintax {
public:
  explicit NicoleSintax() noexcept : Sintax("nc") {}

  [[nodiscard]] auto createLexer() const noexcept -> Lexer override;
};

} // namespace nicole

#endif