#ifndef AST_THIS_H
#define AST_THIS_H

#include "../../../tables/typeTable/types/userTypes/userType.h"
#include "../ast.h"
#include <memory>

namespace nicole {

class AST_THIS final : public AST {
private:
  mutable std::shared_ptr<UserType> userType_{nullptr};

public:
  explicit AST_THIS(const long long unsigned nodeId,
                    const SourceLocation &srcLoc) noexcept
      : AST{nodeId, AST_TYPE::THIS, srcLoc} {}

  ~AST_THIS() noexcept = default;

  [[nodiscard]] auto userType() const noexcept
      -> const std::shared_ptr<UserType> & {
    return userType_;
  }

  auto setUserType(const std::shared_ptr<UserType> &userType) const noexcept
      -> void {
    userType_ = userType;
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

#endif // AST_H
