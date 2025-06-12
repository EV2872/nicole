#ifndef METHOD_H
#define METHOD_H

#include "../../../../parsingAnalysis/ast/functions/parameters.h"
#include "../../../symbol.h"
#include "genericParameter.h"
#include <memory>

namespace nicole {

class AST_BODY;

class Method final : public Symbol {
private:
  mutable std::vector<GenericParameter> generics_;
  mutable Parameters params_;
  mutable std::shared_ptr<Type> returnType_;
  std::shared_ptr<AST_BODY> body_;
  bool isVirtual_;
  mutable bool isInherited_{false};

public:
  explicit Method(const std::string &id,
                  const std::vector<GenericParameter> &generics,
                  const Parameters &params,
                  const std::shared_ptr<Type> &returnType,
                  const std::shared_ptr<AST_BODY> &body,
                  const bool isVirtual) noexcept
      : Symbol{id}, generics_{generics}, params_{params},
        returnType_{returnType}, body_{body}, isVirtual_{isVirtual} {}

  [[nodiscard]] auto generics() const noexcept
      -> const std::vector<GenericParameter> & {
    return generics_;
  }

  [[nodiscard]] auto params() const noexcept -> const Parameters & {
    return params_;
  }

  [[nodiscard]] auto returnType() const noexcept
      -> const std::shared_ptr<Type> & {
    return returnType_;
  }

  [[nodiscard]] auto body() const noexcept
      -> const std::shared_ptr<AST_BODY> & {
    return body_;
  }

  [[nodiscard]] auto isVirtual() const noexcept -> bool { return isVirtual_; }

  auto setInherit(const bool inherit) const noexcept -> void {
    isInherited_ = inherit;
  }

  [[nodiscard]] auto isInherited() const noexcept -> bool {
    return isInherited_;
  }

  auto setReturnType(const std::shared_ptr<Type> &type) const noexcept -> void {
    returnType_ = type;
  }

  auto setParameters(const Parameters &params) const noexcept -> void {
    params_ = params;
  }

  auto setGenerics(const std::vector<GenericParameter> &generics) const noexcept
      -> void {
    generics_ = generics;
  }
};

} // namespace nicole

#endif