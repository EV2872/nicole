#ifndef AST_LOOP_H
#define AST_LOOP_H

#include "../conditionals/ast_condition.h"
#include "../statements/ast_body.h"
#include <memory>

namespace nicole {

class AST_LOOP : public AST {
protected:
  std::shared_ptr<AST_CONDITION> condition_;
  std::shared_ptr<AST_BODY> body_;

public:
  explicit AST_LOOP(const long long unsigned nodeId, const AST_TYPE type,
                    const SourceLocation &srcLoc,
                    const std::shared_ptr<AST_CONDITION> &condition,
                    const std::shared_ptr<AST_BODY> &body) noexcept
      : AST(nodeId, type, srcLoc), condition_{condition}, body_{body} {}

  [[nodiscard]] auto condition() const noexcept
      -> const std::shared_ptr<AST_CONDITION> & {
    return condition_;
  }

  [[nodiscard]] auto body() const noexcept
      -> const std::shared_ptr<AST_BODY> & {
    return body_;
  }
};

} // namespace nicole

#endif // AST_LOOP_H
