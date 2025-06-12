#ifndef AST_VAR_DECL_H
#define AST_VAR_DECL_H

#include "../ast.h"
#include <memory>

namespace nicole {

class AST_VAR_DECL : public AST {
private:
  std::string id_;
  std::shared_ptr<AST> value_;

public:
  explicit AST_VAR_DECL(const long long unsigned nodeId, const AST_TYPE astType,
                        const SourceLocation &srcLoc, const std::string &id,
                        const std::shared_ptr<AST> &value) noexcept
      : AST(nodeId, astType, srcLoc), id_{id}, value_{value} {}

  [[nodiscard]] auto id() const noexcept -> const std::string & { return id_; }

  [[nodiscard]] auto value() const noexcept -> const std::shared_ptr<AST> & {
    return value_;
  }
};

} // namespace nicole

#endif