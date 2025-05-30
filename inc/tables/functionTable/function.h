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

  [[nodiscard]] const std::vector<GenericParameter> &generics() const noexcept {
    return generics_;
  }

  [[nodiscard]] const Parameters &params() const noexcept { return params_; }

  [[nodiscard]] const std::shared_ptr<Type> &returnType() const noexcept {
    return returnType_;
  }

  [[nodiscard]] const std::shared_ptr<AST_BODY> &body() const noexcept {
    return body_;
  }

  [[nodiscard]] const AST_FUNC_DECL *declReference() const noexcept {
    return declReference_;
  }

  [[nodiscard]] const std::shared_ptr<llvm::Function> &
  llvmVersion() const noexcept {
    return llvmVersion_;
  }

  void setDeclReference(const AST_FUNC_DECL *declReference) const noexcept {
    declReference_ = declReference;
  }

  void
  setLlvmVersion(const std::shared_ptr<llvm::Function> &llvmVersion) noexcept {
    llvmVersion_ = llvmVersion;
  }

  void setReturnType(const std::shared_ptr<Type> &type) const noexcept {
    returnType_ = type;
  }

  void setParameters(const Parameters &params) const noexcept {
    params_ = params;
  }
};

} // namespace nicole

#endif