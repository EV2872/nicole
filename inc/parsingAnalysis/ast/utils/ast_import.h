#ifndef AST_IMPORT_H
#define AST_IMPORT_H

#include "../ast.h"
#include <filesystem>
#include <memory>

namespace nicole {

class AST_IMPORT : public AST {
private:
  std::filesystem::path path_;

public:
  explicit AST_IMPORT(const long long unsigned nodeId,
                      const SourceLocation &srcLoc,
                      const std::filesystem::path &path) noexcept
      : AST(nodeId, AST_TYPE::IMPORT, srcLoc), path_{path} {}

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return path_;
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