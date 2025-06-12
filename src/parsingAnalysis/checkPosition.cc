#include "../../inc/parsingAnalysis/checkPosition.h"
#include <cstddef>

namespace nicole {

auto CheckPosition::hasAnyAncestorOf(
    const AST *node, const std::unordered_set<AST_TYPE> &possibles) noexcept
    -> bool {
  const AST *auxiliar{node};
  while (auxiliar->father()) {
    auxiliar = auxiliar->father().get();
    if (possibles.count(auxiliar->type())) {
      return true;
    }
  }
  return false;
}

auto CheckPosition::hasEveryAncestorInOrder(
    const AST *node, const std::vector<AST_TYPE> &possibles) noexcept -> bool {
  const AST *auxiliar{node};
  size_t index{0};
  while (auxiliar->father() and index < possibles.size()) {
    auxiliar = auxiliar->father().get();
    if (auxiliar->type() != possibles[index]) {
      return false;
    }
    ++index;
  }
  return true;
}

auto CheckPosition::itsBodyAncestorHasParent(const AST *node) noexcept -> bool {
  std::shared_ptr<AST> father{nullptr};
  if (!node->father()) {
    return false;
  }
  father = node->father();
  if (father->type() != AST_TYPE::STATEMENT) {
    return false;
  }
  if (!father->father()) {
    return false;
  }
  father = father->father();
  if (father->type() != AST_TYPE::BODY) {
    return false;
  }
  if (!father->father()) {
    return false;
  }
  return true;
}

auto CheckPosition::isInsideForHeader(const AST *node) noexcept -> bool {
  return (node->father() and node->father()->type() == AST_TYPE::FOR) ? true
                                                                      : false;
}

auto CheckPosition::hasLoopAncestor(const AST *node) noexcept -> bool {
  return hasAnyAncestorOf(node,
                          {AST_TYPE::WHILE, AST_TYPE::FOR, AST_TYPE::DO_WHILE});
}

auto CheckPosition::hasAssigmentOrDeclAncestor(const AST *node) noexcept
    -> bool {
  return hasAnyAncestorOf(node, {AST_TYPE::AUTO_DECL, AST_TYPE::VAR_TYPED_DECL,
                                 AST_TYPE::ASSIGNMENT});
}

auto CheckPosition::isOutOfScope(const AST *node) noexcept -> bool {
  const AST *auxiliar{node};
  while (auxiliar->father()) {
    auxiliar = auxiliar->father().get();
    if (auxiliar->type() == AST_TYPE::BODY and auxiliar->father()) {
      return false;
    }
  }
  return true;
}

} // namespace nicole