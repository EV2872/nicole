#ifndef TREE_H
#define TREE_H

#include "statements/ast_body.h"
#include <memory>

namespace nicole {

class Tree final {
private:
  std::shared_ptr<AST_BODY> root_;

public:
  explicit Tree(const std::shared_ptr<AST_BODY> &root) noexcept : root_{root} {}

  [[nodiscard]] auto root() const noexcept
      -> const std::shared_ptr<AST_BODY> & {
    return root_;
  }
};

} // namespace nicole

#endif