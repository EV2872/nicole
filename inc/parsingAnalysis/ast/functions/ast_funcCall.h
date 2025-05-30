#ifndef AST_FUNC_CALL_H
#define AST_FUNC_CALL_H

#include "../../../tables/typeTable/types/type.h"
#include "ast_callable.h"
#include "ast_funcDecl.h"
#include <memory>
#include <vector>

namespace nicole {

class AST_FUNC_CALL : public AST_CALLABLE {
private:
  mutable std::vector<std::shared_ptr<Type>> replaceOfGenerics_;
  std::vector<std::shared_ptr<AST>> parameters_;
  mutable const AST_FUNC_DECL *declReference_{nullptr};

public:
  explicit AST_FUNC_CALL(
      const long long unsigned nodeId, const SourceLocation &srcLoc,
      const std::string &id,
      const std::vector<std::shared_ptr<Type>> &replaceOfGenerics,
      const std::vector<std::shared_ptr<AST>> &parameters) noexcept
      : AST_CALLABLE(nodeId, AST_TYPE::FUN_CALL, srcLoc, id),
        replaceOfGenerics_{replaceOfGenerics}, parameters_{parameters} {}

  [[nodiscard]] const std::vector<std::shared_ptr<Type>> &
  replaceOfGenerics() const noexcept {
    return replaceOfGenerics_;
  }

  [[nodiscard]] std::expected<std::monostate, Error>
  setGenericReplacement(const std::size_t pos,
                        const std::shared_ptr<Type> &type) const noexcept {
    if (pos >= replaceOfGenerics_.size()) {
      return createError(
          ERROR_TYPE::TYPE,
          "trying to access a invalid position in a replacement list");
    }
    replaceOfGenerics_[pos] = type;
    return {};
  }

  [[nodiscard]] const AST_FUNC_DECL *declReference() const noexcept {
    return declReference_;
  }

  void setDeclReference(const AST_FUNC_DECL *declReference) const noexcept {
    declReference_ = declReference;
  }

  [[nodiscard]] const std::vector<std::shared_ptr<AST>> &
  parameters() const noexcept {
    return parameters_;
  }

  [[nodiscard]] std::expected<std::string, Error>
  accept(const PrintTree &visitor) const noexcept override {
    return visitor.visit(this);
  }

  [[nodiscard]] std::expected<bool, Error>
  accept(const ValidateTree &visitor) const noexcept override {
    return visitor.visit(this);
  }

  [[nodiscard]] std::expected<std::monostate, Error>
  accept(const FillSemanticInfo &visitor) const noexcept override {
    return visitor.visit(this);
  }

  [[nodiscard]] std::expected<std::shared_ptr<Type>, Error>
  accept(const TypeAnalysis &visitor) const noexcept override {
    return visitor.visit(this);
  }

  [[nodiscard]] std::expected<std::monostate, Error>
  accept(const Monomorphize &visitor) const noexcept override {
    return visitor.visit(this);
  }

  [[nodiscard]] std::expected<llvm::Value *, Error>
  accept(const CodeGeneration &visitor) const noexcept override {
    return visitor.visit(this);
  }
};

} // namespace nicole

#endif