#ifndef AST_CALLABLE_H
#define AST_CALLABLE_H

#include "../ast.h"

namespace nicole {

class AST_CALLABLE : public AST {
private:
  std::string id_;

public:
  explicit AST_CALLABLE(const long long unsigned nodeId, const AST_TYPE type,
                        const SourceLocation &srcLoc,
                        const std::string &id) noexcept
      : AST{nodeId, type, srcLoc}, id_{id} {}

  [[nodiscard]] auto id() const noexcept -> const std::string & { return id_; }
};

} // namespace nicole

#endif