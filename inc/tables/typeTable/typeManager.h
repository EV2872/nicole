#ifndef TYPE_MANAGER_H
#define TYPE_MANAGER_H

#include "typeTable.h"
#include <memory>

namespace nicole {

class TypeManager final {
private:
  std::shared_ptr<TypeTable> typeTable_;

public:
  explicit TypeManager(const std::shared_ptr<TypeTable> &typeTable) noexcept
      : typeTable_{typeTable} {}

  [[nodiscard]] auto canAssign(const std::shared_ptr<Type> &dest,
                               const std::shared_ptr<Type> &src) const noexcept
      -> bool;
};

} // namespace nicole

#endif
