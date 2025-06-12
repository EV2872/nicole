#ifndef AST_DELETE_H
#define AST_DELETE_H

#include "../ast.h"
#include <memory>

namespace nicole {

class AST_DELETE : public AST {
private:
  std::shared_ptr<AST> value_;

public:
  explicit AST_DELETE(const long long unsigned nodeId,
                      const SourceLocation &srcLoc,
                      const std::shared_ptr<AST> &value) noexcept
      : AST(nodeId, AST_TYPE::DELETE, srcLoc), value_{value} {}

  [[nodiscard]] auto value() const noexcept -> const std::shared_ptr<AST> & {
    return value_;
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