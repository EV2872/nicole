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

  [[nodiscard]] const std::shared_ptr<Type> fatherType() const noexcept {
    return fatherType_;
  }

  [[nodiscard]] const std::vector<std::shared_ptr<AST>>
  arguments() const noexcept {
    return arguments_;
  }

  [[nodiscard]] const std::vector<std::shared_ptr<Type>>
  replacements() const noexcept {
    return replacements_;
  }

  [[nodiscard]] std::expected<std::monostate, Error>
  setGenericReplacement(const std::size_t pos,
                        const std::shared_ptr<Type> &type) const noexcept {
    if (pos >= replacements_.size()) {
      return createError(
          ERROR_TYPE::TYPE,
          "trying to access a invalid position in a replacement list");
    }
    replacements_[pos] = type;
    return {};
  }

  void setFatherType(const std::shared_ptr<Type> &fatherType) const noexcept {
    fatherType_ = fatherType;
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