#ifndef AST_ENUM_H
#define AST_ENUM_H

#include "../ast.h"
#include <string>
#include <vector>

namespace nicole {

class AST_ENUM final : public AST {
private:
  std::string id_;
  std::vector<std::string> enumIdentifiers_;

public:
  explicit AST_ENUM(const long long unsigned nodeId,
                    const SourceLocation &srcLoc, const std::string &id,
                    const std::vector<std::string> &enumIdentifiers) noexcept
      : AST(nodeId, AST_TYPE::ENUM, srcLoc), id_{id},
        enumIdentifiers_{enumIdentifiers} {}

  [[nodiscard]] auto id() const noexcept -> const std::string & { return id_; }

  [[nodiscard]] auto identifiers() const noexcept
      -> const std::vector<std::string> & {
    return enumIdentifiers_;
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