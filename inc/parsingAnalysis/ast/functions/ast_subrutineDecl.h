#ifndef AST_SUBRUTINE_DECL_H
#define AST_SUBRUTINE_DECL_H

#include "../../../tables/typeTable/types/type.h"
#include "../statements/ast_body.h"
#include <memory>
#include <string>

namespace nicole {

class AST_SUBRUTINE_DECL : public AST {
private:
  mutable std::string id_;
  mutable std::shared_ptr<Type> returnType_;
  mutable std::shared_ptr<AST_BODY> body_;
  mutable std::string mangledId_{""};

public:
  explicit AST_SUBRUTINE_DECL(const long long unsigned nodeId,
                              const AST_TYPE type, const SourceLocation &srcLoc,
                              const std::string &id,
                              const std::shared_ptr<Type> &returnType,
                              const std::shared_ptr<AST_BODY> &body) noexcept
      : AST(nodeId, type, srcLoc), id_{id}, returnType_{returnType},
        body_{body} {}

  [[nodiscard]] auto id() const noexcept -> const std::string & { return id_; }

  [[nodiscard]] auto mangledId() const noexcept -> const std::string & {
    return mangledId_;
  }

  [[nodiscard]] auto returnType() const noexcept
      -> const std::shared_ptr<Type> & {
    return returnType_;
  }

  [[nodiscard]] auto body() const noexcept
      -> const std::shared_ptr<AST_BODY> & {
    return body_;
  }

  auto setReturnType(const std::shared_ptr<Type> &type) const noexcept -> void {
    returnType_ = type;
  }

  auto setId(const std::string &id) const noexcept -> void { id_ = id; }

  auto setMangledId(const std::string &id) const noexcept -> void {
    mangledId_ = id;
  }
};

} // namespace nicole

#endif