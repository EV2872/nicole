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

  [[nodiscard]] const std::string &id() const noexcept { return id_; }

  [[nodiscard]] const std::string &mangledId() const noexcept {
    return mangledId_;
  }

  [[nodiscard]] const std::shared_ptr<Type> &returnType() const noexcept {
    return returnType_;
  }

  [[nodiscard]] const std::shared_ptr<AST_BODY> &body() const noexcept {
    return body_;
  }

  void setReturnType(const std::shared_ptr<Type> &type) const noexcept {
    returnType_ = type;
  }

  void setId(const std::string &id) const noexcept { id_ = id; }

  void setMangledId(const std::string &id) const noexcept { mangledId_ = id; }
};

} // namespace nicole

#endif