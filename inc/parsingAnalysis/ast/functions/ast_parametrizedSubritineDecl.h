#ifndef AST_PARAMETRIZED_SUBRUTINE_DECL_H
#define AST_PARAMETRIZED_SUBRUTINE_DECL_H

#include "ast_subrutineDecl.h"
#include "parameters.h"
#include <memory>
#include <string>

namespace nicole {

class AST_PARAMETRIZED_SUBRUTINE_DECL : public AST_SUBRUTINE_DECL {
private:
  mutable Parameters params_;

public:
  explicit AST_PARAMETRIZED_SUBRUTINE_DECL(
      const long long unsigned nodeId, const AST_TYPE type,
      const SourceLocation &srcLoc, const std::string &id,
      const Parameters &params, const std::shared_ptr<Type> &returnType,
      const std::shared_ptr<AST_BODY> &body) noexcept
      : AST_SUBRUTINE_DECL(nodeId, type, srcLoc, id, returnType, body),
        params_{params} {}

  [[nodiscard]] auto parameters() const noexcept -> const Parameters & {
    return params_;
  }

  auto setParameters(const Parameters &params) const noexcept -> void {
    params_ = params;
  }
};

} // namespace nicole

#endif