#ifndef AST_H
#define AST_H

#include "../../lexicalAnalysis/sourceLocation.h"
#include "../../visitors/codeGeneration/codeGeneration.h"
#include "../../visitors/fillSemanticInfo/fillSemanticInfo.h"
#include "../../visitors/monomorphize/monomorphize.h"
#include "../../visitors/printTree/printTree.h"
#include "../../visitors/typeAnalysis/typeAnalysis.h"
#include "../../visitors/validateTree/validateTree.h"
#include "astType.h"
#include <memory>
#include <variant>

namespace nicole {

class AST : public std::enable_shared_from_this<AST> {
private:
  long long unsigned nodeId_;
  AST_TYPE type_;
  std::weak_ptr<AST> father_;
  SourceLocation srcLoc_;
  mutable std::shared_ptr<Type> returnedFromTypeAnalysis_{nullptr};

public:
  explicit AST(const long long unsigned nodeId, const AST_TYPE type,
               const SourceLocation &srcLoc) noexcept
      : nodeId_{nodeId}, type_{type}, srcLoc_{srcLoc} {}

  virtual ~AST() noexcept = default;

  [[nodiscard]] auto nodeId() const noexcept -> long long unsigned {
    return nodeId_;
  }

  [[nodiscard]] auto type() const noexcept -> AST_TYPE { return type_; }

  [[nodiscard]] auto father() const noexcept -> const std::shared_ptr<AST> {
    return father_.lock();
  }

  [[nodiscard]] auto srcLoc() const noexcept -> const SourceLocation & {
    return srcLoc_;
  }

  [[nodiscard]] auto returnedFromTypeAnalysis() const noexcept
      -> const std::shared_ptr<Type> & {
    return returnedFromTypeAnalysis_;
  }

  auto setFather(const std::shared_ptr<AST> &father) noexcept -> void {
    father_ = father;
  }

  auto setReturnedFromAnalysis(const std::shared_ptr<Type> &type) const noexcept
      -> void {
    returnedFromTypeAnalysis_ = type;
  }

  [[nodiscard]] virtual auto accept(const PrintTree &visitor) const noexcept
      -> std::expected<std::string, Error> = 0;

  [[nodiscard]] virtual auto accept(const ValidateTree &visitor) const noexcept
      -> std::expected<bool, Error> = 0;

  [[nodiscard]] virtual auto
  accept(const FillSemanticInfo &visitor) const noexcept
      -> std::expected<std::monostate, Error> = 0;

  [[nodiscard]] virtual auto accept(const TypeAnalysis &visitor) const noexcept
      -> std::expected<std::shared_ptr<Type>, Error> = 0;

  [[nodiscard]] virtual auto accept(const Monomorphize &visitor) const noexcept
      -> std::expected<std::monostate, Error> = 0;

  [[nodiscard]] virtual auto
  accept(const CodeGeneration &visitor) const noexcept
      -> std::expected<llvm::Value *, Error> = 0;
};

} // namespace nicole

#endif // AST_H
