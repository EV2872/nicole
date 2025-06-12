#ifndef NAME_MANGLER_H
#define NAME_MANGLER_H

#include "../errors.h"
#include "../parsingAnalysis/ast/functions/ast_funcDecl.h"
#include "../parsingAnalysis/ast/userTypes/ast_struct.h"
#include <expected>
#include <memory>
#include <string>

namespace nicole {

class NameMangler final {
private:
  inline static std::string prefix_;

  NameMangler() noexcept = delete;

public:
  [[nodiscard]] static auto
  mangleFunction(const std::shared_ptr<AST_FUNC_DECL> &node) noexcept
      -> std::expected<std::string, Error>;

  [[nodiscard]] static auto
  mangleClass(const std::shared_ptr<AST_STRUCT> &node) noexcept
      -> std::expected<std::string, Error>;

  [[nodiscard]] static auto
  mangleConstructor(const std::shared_ptr<AST_CONSTRUCTOR_DECL> &node,
                    const std::string &constructorOwner) noexcept
      -> std::expected<std::string, Error>;

  [[nodiscard]] static auto
  mangleDestructor(const std::shared_ptr<AST_DESTRUCTOR_DECL> &node,
                   const std::string &desstructorOwner) noexcept
      -> std::expected<std::string, Error>;

  [[nodiscard]] static auto
  mangleMethod(const std::shared_ptr<AST_METHOD_DECL> &node,
               const std::string &methodOwner) noexcept
      -> std::expected<std::string, Error>;
};

} // namespace nicole

#endif