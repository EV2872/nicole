#ifndef ERROR_TABLE_H
#define ERROR_TABLE_H

#include "../../errors.h"
#include "../../lexicalAnalysis/sourceLocation.h"
#include <cstddef>
#include <expected>
#include <map>

namespace nicole {

class ErrorTable {
private:
  mutable std::map<SourceLocation, Error> errors_;

public:
  [[nodiscard]] auto size() const noexcept -> std::size_t {
    return errors_.size();
  }

  auto insert(const SourceLocation &srcLoc, const Error &err) const noexcept
      -> void {
    errors_[srcLoc] = err;
  }

  [[nodiscard]] auto begin() const noexcept -> decltype(errors_.begin()) {
    return errors_.begin();
  }

  [[nodiscard]] auto end() const noexcept -> decltype(errors_.end()) {
    return errors_.end();
  }
};

} // namespace nicole

#endif