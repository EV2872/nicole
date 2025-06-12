#ifndef NICOLE_H
#define NICOLE_H

#include "compiler.h"

namespace nicole {

class Nicole final : public Compiler {
public:
  explicit Nicole(const std::shared_ptr<Sintax> &sintax) noexcept
      : Compiler(sintax) {}

  [[nodiscard]] auto compile(const Options &options) const noexcept
      -> std::expected<std::monostate, Error> override;
};

} // namespace nicole

#endif