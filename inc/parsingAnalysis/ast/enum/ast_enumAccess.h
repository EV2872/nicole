#ifndef AST_ENUM_ACCESS_H
#define AST_ENUM_ACCESS_H

#include "../functions/ast_callable.h"
#include <string>

namespace nicole {

class AST_ENUM_ACCESS final : public AST_CALLABLE {
private:
  std::string identifier_;

public:
  explicit AST_ENUM_ACCESS(const long long unsigned nodeId,
                           const SourceLocation &srcLoc,
                           const std::string &enumId,
                           const std::string &identifier) noexcept
      : AST_CALLABLE{nodeId, AST_TYPE::ENUM_ACCESS, srcLoc, enumId},
        identifier_{identifier} {}

  [[nodiscard]] auto identifier() const noexcept -> const std::string & {
    return identifier_;
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