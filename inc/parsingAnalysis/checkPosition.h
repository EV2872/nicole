#ifndef CHECK_POSITION_H
#define CHECK_POSITION_H

#include "ast/ast.h"
#include <unordered_set>
#include <vector>

namespace nicole {

class CheckPosition final {
private:
  ~CheckPosition() = delete;

public:
  [[nodiscard]] static auto
  hasAnyAncestorOf(const AST *node,
                   const std::unordered_set<AST_TYPE> &possibles) noexcept
      -> bool;

  [[nodiscard]] static auto hasEveryAncestorInOrder(
      const AST *node, const std::vector<AST_TYPE> &possibles) noexcept -> bool;

  [[nodiscard]] static auto itsBodyAncestorHasParent(const AST *node) noexcept
      -> bool;

  [[nodiscard]] static auto isInsideForHeader(const AST *node) noexcept -> bool;

  [[nodiscard]] static auto hasLoopAncestor(const AST *node) noexcept -> bool;

  [[nodiscard]] static auto hasAssigmentOrDeclAncestor(const AST *node) noexcept
      -> bool;

  [[nodiscard]] static auto isOutOfScope(const AST *node) noexcept -> bool;
};

} // namespace nicole

#endif