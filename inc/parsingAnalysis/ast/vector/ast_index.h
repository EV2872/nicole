#ifndef AST_INDEX_H
#define AST_INDEX_H

#include "../ast.h"

namespace nicole {

class AST_INDEX final : public AST {
private:
  std::shared_ptr<AST> index_;

public:
  explicit AST_INDEX(const long long unsigned nodeId,
                     const SourceLocation &srcLoc,
                     const std::shared_ptr<AST> &index) noexcept
      : AST{nodeId, AST_TYPE::INDEX, srcLoc}, index_{index} {}

  ~AST_INDEX() noexcept = default;

  [[nodiscard]] const std::shared_ptr<AST> &index() const noexcept {
    return index_;
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

#endif // AST_H
