#ifndef AST_METHOD_CALL_H
#define AST_METHOD_CALL_H

#include "../../../tables/typeTable/types/type.h"
#include "../functions/ast_callable.h"
#include <memory>
#include <vector>

namespace nicole {

class AST_METHOD_CALL : public AST_CALLABLE {
private:
  mutable std::vector<std::shared_ptr<Type>> replaceOfGenerics_;
  std::vector<std::shared_ptr<AST>> parameters_;

public:
  explicit AST_METHOD_CALL(
      const long long unsigned nodeId, const SourceLocation &srcLoc,
      const std::string &id,
      const std::vector<std::shared_ptr<Type>> &replaceOfGenerics,
      const std::vector<std::shared_ptr<AST>> &parameters) noexcept
      : AST_CALLABLE(nodeId, AST_TYPE::METHOD_CALL, srcLoc, id),
        replaceOfGenerics_{replaceOfGenerics}, parameters_{parameters} {}

  [[nodiscard]] auto replaceOfGenerics() const noexcept
      -> const std::vector<std::shared_ptr<Type>> & {
    return replaceOfGenerics_;
  }

  [[nodiscard]] auto parameters() const noexcept
      -> const std::vector<std::shared_ptr<AST>> & {
    return parameters_;
  }

  [[nodiscard]] auto
  setGenericReplacement(const std::size_t pos,
                        const std::shared_ptr<Type> &type) const noexcept
      -> std::expected<std::monostate, Error> {
    if (pos >= replaceOfGenerics_.size()) {
      return createError(
          ERROR_TYPE::TYPE,
          "trying to access a invalid position in a replacement list");
    }
    replaceOfGenerics_[pos] = type;
    return {};
  }

  [[nodiscard]] auto accept(const PrintTree &visitor) const noexcept
      -> std::expected<std::string, Error> override {
    return visitor.visit(this);
  }

  [[nodiscard]] auto accept(const ValidateTree &visitor) const noexcept
      -> std::expected<bool, Error> override {
    return visitor.visit(this);
  }

  [[nodiscard]] auto accept(const FillSemanticInfo &visitor) const noexcept
      -> std::expected<std::monostate, Error> override {
    return visitor.visit(this);
  }

  [[nodiscard]] auto accept(const TypeAnalysis &visitor) const noexcept
      -> std::expected<std::shared_ptr<Type>, Error> override {
    return visitor.visit(this);
  }

  [[nodiscard]] auto accept(const Monomorphize &visitor) const noexcept
      -> std::expected<std::monostate, Error> override {
    return visitor.visit(this);
  }

  [[nodiscard]] auto accept(const CodeGeneration &visitor) const noexcept
      -> std::expected<llvm::Value *, Error> override {
    return visitor.visit(this);
  }
};

} // namespace nicole

#endif