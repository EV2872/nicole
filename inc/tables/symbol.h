#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>

namespace nicole {

class Symbol {
protected:
  std::string id_;

public:
  explicit Symbol(const std::string &id) noexcept : id_{id} {}

  [[nodiscard]] auto id() const noexcept -> const std::string & { return id_; }
};

} // namespace nicole

#endif