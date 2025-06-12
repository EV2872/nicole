#ifndef AST_SUPER_H
#define AST_SUPER_H

#include "../ast.h"
#include <memory>

namespace nicole {

class AST_SUPER final : public AST {
private:
  mutable std::shared_ptr<Type> fatherType_;
  mutable std::vector<std::shared_ptr<Type>> replacements_;
  std::vector<std::shared_ptr<AST>> arguments_;

public:
  explicit AST_SUPER(
      const long long unsigned nodeId, const SourceLocation &srcLoc,
      const std::shared_ptr<Type> &fatherType,
      const std::vector<std::shared_ptr<Type>> &replacements,
      const std::vector<std::shared_ptr<AST>> &arguments) noexcept
      : AST{nodeId, AST_TYPE::SUPER, srcLoc}, fatherType_{fatherType},
        replacements_{replacements}, arguments_{arguments} {}

  [[nodiscard]] auto fatherType() const noexcept
      -> const std::shared_ptr<Type> {
    return fatherType_;
  }

  [[nodiscard]] auto arguments() const noexcept
      -> const std::vector<std::shared_ptr<AST>> {
    return arguments_;
  }

  [[nodiscard]] auto replacements() const noexcept
      -> const std::vector<std::shared_ptr<Type>> {
    return replacements_;
  }

  [[nodiscard]] auto
  setGenericReplacement(const std::size_t pos,
                        const std::shared_ptr<Type> &type) const noexcept
      -> std::expected<std::monostate, Error> {
    if (pos >= replacements_.size()) {
      return createError(
          ERROR_TYPE::TYPE,
          "trying to access a invalid position in a replacement list");
    }
    replacements_[pos] = type;
    return {};
  }

  auto setFatherType(const std::shared_ptr<Type> &fatherType) const noexcept
      -> void {
    fatherType_ = fatherType;
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