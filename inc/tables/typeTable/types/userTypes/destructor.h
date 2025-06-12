#ifndef DESTRUCTOR_H
#define DESTRUCTOR_H

#include "../../../symbol.h"
#include "../specialTypes/voidType.h"
#include "genericParameter.h"
#include <memory>

namespace nicole {

class AST_BODY;

class Destructor final : public Symbol {
private:
  std::vector<GenericParameter> generics_;
  std::shared_ptr<Type> returnType_;
  std::shared_ptr<AST_BODY> body_;

public:
  explicit Destructor(const std::string &id,
                      const std::shared_ptr<AST_BODY> &body) noexcept
      : Symbol{id}, returnType_{std::make_shared<VoidType>()}, body_{body} {}

  [[nodiscard]] auto generics() const noexcept
      -> const std::vector<GenericParameter> & {
    return generics_;
  }

  [[nodiscard]] auto returnType() const noexcept
      -> const std::shared_ptr<Type> & {
    return returnType_;
  }

  [[nodiscard]] auto body() const noexcept
      -> const std::shared_ptr<AST_BODY> & {
    return body_;
  }
};

} // namespace nicole

#endif