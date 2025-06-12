#ifndef FUNCTION_H
#define FUNCTION_H

#include "../../parsingAnalysis/ast/functions/parameters.h"
#include "../symbol.h"
#include "../typeTable/types/userTypes/genericParameter.h"
#include <llvm/IR/Function.h>
#include <memory>

namespace nicole {

class AST_BODY;
class AST_FUNC_DECL;

class Function final : public Symbol {
private:
  std::vector<GenericParameter> generics_;
  mutable Parameters params_;
  mutable std::shared_ptr<Type> returnType_;
  std::shared_ptr<AST_BODY> body_;
  mutable std::shared_ptr<llvm::Function> llvmVersion_;
  mutable const AST_FUNC_DECL *declReference_;

public:
  explicit Function(const std::string &id,
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

  [[nodiscard]] auto declReference() const noexcept -> const AST_FUNC_DECL * {
    return declReference_;
  }

  [[nodiscard]] auto llvmVersion() const noexcept
      -> const std::shared_ptr<llvm::Function> & {
    return llvmVersion_;
  }

  auto setDeclReference(const AST_FUNC_DECL *declReference) const noexcept
      -> void {
    declReference_ = declReference;
  }

  auto
  setLlvmVersion(const std::shared_ptr<llvm::Function> &llvmVersion) noexcept
      -> void {
    llvmVersion_ = llvmVersion;
  }

  auto setReturnType(const std::shared_ptr<Type> &type) const noexcept -> void {
    returnType_ = type;
  }

  auto setParameters(const Parameters &params) const noexcept -> void {
    params_ = params;
  }
};

} // namespace nicole

#endif