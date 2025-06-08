#ifndef NAME_MANGLER_H
#define NAME_MANGLER_H

#include "../errors.h"
#include "../parsingAnalysis/ast/functions/ast_funcDecl.h"
#include "../parsingAnalysis/ast/userTypes/ast_struct.h"
#include <expected>
#include <memory>
#include <string>

namespace nicole {

class NameMangler {
private:
  inline static std::string prefix_;

  NameMangler() noexcept = delete;

public:
  [[nodiscard]] static std::expected<std::string, Error>
  mangleFunction(const std::shared_ptr<AST_FUNC_DECL> &node) noexcept;

  [[nodiscard]] static std::expected<std::string, Error>
  mangleClass(const std::shared_ptr<AST_STRUCT> &node) noexcept;

  [[nodiscard]] static std::expected<std::string, Error>
  mangleConstructor(const std::shared_ptr<AST_CONSTRUCTOR_DECL> &node,
                    const std::string &constructorOwner) noexcept;

  [[nodiscard]] static std::expected<std::string, Error>
  mangleDestructor(const std::shared_ptr<AST_DESTRUCTOR_DECL> &node,
                   const std::string &desstructorOwner) noexcept;

  [[nodiscard]] static std::expected<std::string, Error>
  mangleMethod(const std::shared_ptr<AST_METHOD_DECL> &node,
               const std::string &methodOwner) noexcept;
};

} // namespace nicole

#endif