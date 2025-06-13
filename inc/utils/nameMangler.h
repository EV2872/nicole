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
  inline static std::string prefix_{"$_"};

  NameMangler() noexcept = delete;

  [[nodiscard]] static auto
  nameMangling(const std::shared_ptr<Type> &type) noexcept
      -> std::expected<std::string, Error>;

  [[nodiscard]] static auto nameManglingImpl(const std::shared_ptr<Type> &type,
                                             std::string &result) noexcept
      -> std::expected<std::string, Error>;

public:
  [[nodiscard]] static auto mangleFunction(
      const std::shared_ptr<AST_FUNC_DECL> &node,
      const std::vector<std::shared_ptr<Type>> &replacements) noexcept
      -> std::expected<std::string, Error>;

  [[nodiscard]] static auto
  mangleClass(const std::shared_ptr<AST_STRUCT> &node,
              const std::vector<std::shared_ptr<Type>> &replacements) noexcept
      -> std::expected<std::string, Error>;

  [[nodiscard]] static auto mangleConstructor(
      const std::shared_ptr<AST_CONSTRUCTOR_DECL> &node,
      const std::string &constructorOwner,
      const std::vector<std::shared_ptr<Type>> &replacements) noexcept
      -> std::expected<std::string, Error>;

  [[nodiscard]] static auto
  mangleDestructor(const std::shared_ptr<AST_DESTRUCTOR_DECL> &node,
                   const std::string &desstructorOwner) noexcept
      -> std::expected<std::string, Error>;

  [[nodiscard]] static auto
  mangleMethod(const std::shared_ptr<AST_METHOD_DECL> &node,
               const std::string &methodOwner,
               const std::vector<std::shared_ptr<Type>> &replacements) noexcept
      -> std::expected<std::string, Error>;
};

} // namespace nicole

#endif