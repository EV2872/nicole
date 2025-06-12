#ifndef CONSTRUCTOR_H
#define CONSTRUCTOR_H

#include "../../../../parsingAnalysis/ast/functions/parameters.h"
#include "../../../symbol.h"
#include "genericParameter.h"
#include <memory>

namespace nicole {

class AST_BODY;

class Constructor final : public Symbol {
private:
  std::vector<GenericParameter> generics_;
  mutable Parameters params_;
  mutable std::shared_ptr<Type> returnType_;
  std::shared_ptr<AST_BODY> body_;

public:
  explicit Constructor(const std::string &id,
                       const std::vector<GenericParameter> &generics,
                       const Parameters &params,
                       const std::shared_ptr<Type> &returnType,
                       const std::shared_ptr<AST_BODY> &body) noexcept
      : Symbol{id}, generics_{generics}, params_{params},
        returnType_{returnType}, body_{body} {}

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

  auto setParams(const Parameters &newParams) const noexcept -> void {
    params_ = newParams;
  }

  auto setReturnType(const std::shared_ptr<Type> &newReturnType) const noexcept
      -> void {
    returnType_ = newReturnType;
  }
};

} // namespace nicole

#endif
