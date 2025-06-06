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

  [[nodiscard]] const std::string &identifier() const noexcept {
    return identifier_;
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

  [[nodiscard]] std::expected<llvm::Value*, Error>
  accept(const CodeGeneration &visitor) const noexcept override {
    return visitor.visit(this);
  }
};

} // namespace nicole

#endif