#ifndef BUILDER_H
#define BUILDER_H

#include "ast/assignments/ast_assignment.h"

#include "ast/conditionals/ast_condition.h"
#include "ast/conditionals/ast_default.h"
#include "ast/functions/ast_funcCall.h"
#include "ast/functions/ast_funcDecl.h"
#include "ast/functions/ast_return.h"

#include "ast/literals/ast_bool.h"
#include "ast/literals/ast_char.h"
#include "ast/literals/ast_double.h"
#include "ast/literals/ast_float.h"
#include "ast/literals/ast_int.h"
#include "ast/literals/ast_null.h"
#include "ast/literals/ast_string.h"

#include "ast/operators/ast_binary.h"

#include "ast/operators/ast_unary.h"

#include "ast/userTypes/ast_constructorCall.h"
#include "ast/userTypes/ast_constructorDecl.h"
#include "ast/userTypes/ast_destructorDecl.h"
#include "ast/userTypes/ast_methodDecl.h"
#include "ast/userTypes/ast_super.h"
#include "ast/userTypes/ast_this.h"
#include "ast/vector/ast_index.h"
#include "ast/vector/ast_vector.h"

#include "ast/enum/ast_enum.h"
#include "ast/enum/ast_enumAccess.h"
#include "ast/userTypes/ast_attrAccess.h"
#include "ast/userTypes/ast_methodCall.h"
#include "ast/userTypes/ast_struct.h"

#include "ast/utils/ast_import.h"
#include "ast/utils/ast_print.h"

#include "ast/statements/ast_body.h"
#include "ast/statements/ast_statement.h"

#include "ast/loops/ast_doWhile.h"
#include "ast/loops/ast_for.h"
#include "ast/loops/ast_pass.h"
#include "ast/loops/ast_stop.h"
#include "ast/loops/ast_while.h"

#include "ast/conditionals/ast_case.h"
#include "ast/conditionals/ast_if.h"
#include "ast/conditionals/ast_switch.h"
#include "ast/conditionals/ast_ternary.h"

#include "ast/variables/ast_autoDecl.h"
#include "ast/variables/ast_typedDecl.h"
#include "ast/variables/ast_varCall.h"

#include "ast/pointer/ast_delete.h"
#include "ast/pointer/ast_deref.h"
#include "ast/pointer/ast_new.h"

#include "ast/chained/ast_chained.h"

#include "ast/tree.h"

#include "../errors.h"

#include <expected>
#include <memory>

namespace nicole {

class Builder final {
private:
  [[nodiscard]] static auto generateNextId() noexcept -> long long unsigned {
    static long long unsigned id{0};
    return ++id;
  }

  Builder() noexcept = delete;

public:
  // Literals
  [[nodiscard]] static auto createBool(const SourceLocation &srcLoc,
                                       const bool value) noexcept
      -> std::expected<std::shared_ptr<AST_BOOL>, Error>;

  [[nodiscard]] static auto createChar(const SourceLocation &srcLoc,
                                       const std::string &value) noexcept
      -> std::expected<std::shared_ptr<AST_CHAR>, Error>;

  [[nodiscard]] static auto createDouble(const SourceLocation &srcLoc,
                                         const double value) noexcept
      -> std::expected<std::shared_ptr<AST_DOUBLE>, Error>;

  [[nodiscard]] static auto createFloat(const SourceLocation &srcLoc,
                                        const float value) noexcept
      -> std::expected<std::shared_ptr<AST_FLOAT>, Error>;

  [[nodiscard]] static auto createInt(const SourceLocation &srcLoc,
                                      const int value) noexcept
      -> std::expected<std::shared_ptr<AST_INT>, Error>;

  [[nodiscard]] static auto createNull(const SourceLocation &srcLoc) noexcept
      -> std::expected<std::shared_ptr<AST_NULL>, Error>;

  [[nodiscard]] static auto createString(const SourceLocation &srcLoc,
                                         const std::string value) noexcept
      -> std::expected<std::shared_ptr<AST_STRING>, Error>;

  // Vectors
  [[nodiscard]] static auto
  createVector(const SourceLocation &srcLoc,
               const std::vector<std::shared_ptr<AST>> values) noexcept
      -> std::expected<std::shared_ptr<AST_VECTOR>, Error>;

  [[nodiscard]] static auto
  createIndex(const SourceLocation &srcLoc,
              const std::shared_ptr<AST> value) noexcept
      -> std::expected<std::shared_ptr<AST_INDEX>, Error>;

  // Pointers
  [[nodiscard]] static auto
  createDelete(const SourceLocation &srcLoc,
               const std::shared_ptr<AST> &value) noexcept
      -> std::expected<std::shared_ptr<AST_DELETE>, Error>;

  [[nodiscard]] static auto
  createNew(const SourceLocation &srcLoc,
            const std::shared_ptr<AST> &value) noexcept
      -> std::expected<std::shared_ptr<AST_NEW>, Error>;

  [[nodiscard]] static auto
  createDeref(const SourceLocation &srcLoc,
              const std::shared_ptr<AST> &value) noexcept
      -> std::expected<std::shared_ptr<AST_DEREF>, Error>;

  // Binary
  [[nodiscard]] static auto
  createBinary(const SourceLocation &srcLoc, const Token &op,
               const std::shared_ptr<AST> &left,
               const std::shared_ptr<AST> &right) noexcept
      -> std::expected<std::shared_ptr<AST_BINARY>, Error>;

  // Unary
  [[nodiscard]] static auto
  createUnary(const SourceLocation &srcLoc, const Token &op,
              const std::shared_ptr<AST> &value) noexcept
      -> std::expected<std::shared_ptr<AST_UNARY>, Error>;

  // Asignment

  [[nodiscard]] static auto
  createAssignment(const SourceLocation &srcLoc, const Token &op,
                   const std::shared_ptr<AST> &left,
                   const std::shared_ptr<AST> &value) noexcept
      -> std::expected<std::shared_ptr<AST_ASSIGNMENT>, Error>;

  // Utils
  [[nodiscard]] static auto
  createPrint(const SourceLocation &srcLoc,
              const std::vector<std::shared_ptr<AST>> &values) noexcept
      -> std::expected<std::shared_ptr<AST_PRINT>, Error>;

  [[nodiscard]] static auto
  createImport(const SourceLocation &srcLoc,
               const std::filesystem::path &path) noexcept
      -> std::expected<std::shared_ptr<AST_IMPORT>, Error>;

  // Statements
  [[nodiscard]] static auto
  createStatement(const SourceLocation &srcLoc,
                  const std::shared_ptr<AST> &expression) noexcept
      -> std::expected<std::shared_ptr<AST_STATEMENT>, Error>;

  [[nodiscard]] static auto
  createBody(const SourceLocation &srcLoc,
             const std::vector<std::shared_ptr<AST_STATEMENT>> &body) noexcept
      -> std::expected<std::shared_ptr<AST_BODY>, Error>;

  // Loops
  [[nodiscard]] static auto
  createWhile(const SourceLocation &srcLoc,
              const std::shared_ptr<AST_CONDITION> &condition,
              const std::shared_ptr<AST_BODY> &body) noexcept
      -> std::expected<std::shared_ptr<AST_WHILE>, Error>;

  [[nodiscard]] static auto
  createFor(const SourceLocation &srcLoc,
            const std::vector<std::shared_ptr<AST>> &init,
            const std::shared_ptr<AST_CONDITION> &condition,
            const std::vector<std::shared_ptr<AST>> &update,
            const std::shared_ptr<AST_BODY> &body) noexcept
      -> std::expected<std::shared_ptr<AST_FOR>, Error>;

  [[nodiscard]] static auto
  createDoWhile(const SourceLocation &srcLoc,
                const std::shared_ptr<AST_BODY> &body,
                const std::shared_ptr<AST_CONDITION> &condition) noexcept
      -> std::expected<std::shared_ptr<AST_DO_WHILE>, Error>;

  [[nodiscard]] static auto createPass(const SourceLocation &srcLoc) noexcept
      -> std::expected<std::shared_ptr<AST_PASS>, Error>;

  [[nodiscard]] static auto createStop(const SourceLocation &srcLoc) noexcept
      -> std::expected<std::shared_ptr<AST_STOP>, Error>;

  // Conditionals
  [[nodiscard]] static auto
  createIf(const SourceLocation &srcLoc,
           const std::shared_ptr<AST_CONDITION> &condition,
           const std::shared_ptr<AST_BODY> &body,
           const std::vector<std::shared_ptr<AST_ELSE_IF>> &elseIf,
           const std::shared_ptr<AST_BODY> &elseBody) noexcept
      -> std::expected<std::shared_ptr<AST_IF>, Error>;

  [[nodiscard]] static auto
  createElseIf(const SourceLocation &srcLoc,
               const std::shared_ptr<AST_CONDITION> &condition,
               const std::shared_ptr<AST_BODY> &body) noexcept
      -> std::expected<std::shared_ptr<AST_ELSE_IF>, Error>;

  [[nodiscard]] static auto
  createSwitch(const SourceLocation &srcLoc,
               const std::shared_ptr<AST_CONDITION> &condition,
               const std::vector<std::shared_ptr<AST_CASE>> &cases,
               const std::shared_ptr<AST_DEFAULT> &default_) noexcept
      -> std::expected<std::shared_ptr<AST_SWITCH>, Error>;

  [[nodiscard]] static auto
  createCase(const SourceLocation &srcLoc, const std::shared_ptr<AST> &match,
             const std::shared_ptr<AST_BODY> &body) noexcept
      -> std::expected<std::shared_ptr<AST_CASE>, Error>;

  [[nodiscard]] static auto
  createDefault(const SourceLocation &srcLoc,
                const std::shared_ptr<AST_BODY> &body) noexcept
      -> std::expected<std::shared_ptr<AST_DEFAULT>, Error>;

  [[nodiscard]] static auto
  createTernary(const SourceLocation &srcLoc,
                const std::shared_ptr<AST_CONDITION> &condition,
                const std::shared_ptr<AST> &first,
                const std::shared_ptr<AST> &second) noexcept
      -> std::expected<std::shared_ptr<AST_TERNARY>, Error>;

  [[nodiscard]] static auto
  createCondition(const SourceLocation &srcLoc,
                  const std::shared_ptr<AST> &expression) noexcept
      -> std::expected<std::shared_ptr<AST_CONDITION>, Error>;

  // Functions
  [[nodiscard]] static auto
  createFunCall(const SourceLocation &srcLoc, const std::string &id,
                const std::vector<std::shared_ptr<Type>> &replaceOfGenerics,
                const std::vector<std::shared_ptr<AST>> &parameters) noexcept
      -> std::expected<std::shared_ptr<AST_FUNC_CALL>, Error>;

  [[nodiscard]] static auto
  createFuncDecl(const SourceLocation &srcLoc, const std::string &id,
                 const std::vector<GenericParameter> &generics,
                 const Parameters &params,
                 const std::shared_ptr<Type> &returnType,
                 const std::shared_ptr<AST_BODY> &body) noexcept
      -> std::expected<std::shared_ptr<AST_FUNC_DECL>, Error>;

  [[nodiscard]] static auto
  createReturn(const SourceLocation &srcLoc,
               const std::shared_ptr<AST> &value) noexcept
      -> std::expected<std::shared_ptr<AST_RETURN>, Error>;

  // Enum
  [[nodiscard]] static auto
  createEnum(const SourceLocation &srcLoc, const std::string &id,
             const std::vector<std::string> &enumIdentifiers) noexcept
      -> std::expected<std::shared_ptr<AST_ENUM>, Error>;

  [[nodiscard]] static auto
  createEnumAccess(const SourceLocation &srcLoc, const std::string &id,
                   const std::string &identifiers) noexcept
      -> std::expected<std::shared_ptr<AST_ENUM_ACCESS>, Error>;

  // Usert types
  [[nodiscard]] static auto
  createStruct(const SourceLocation &srcLoc, const std::string &id,
               const std::vector<GenericParameter> &generics,
               const std::shared_ptr<Type> &fatherType,
               const Attributes &attributes,
               const std::vector<std::shared_ptr<AST_METHOD_DECL>> &methods,
               const std::shared_ptr<AST_CONSTRUCTOR_DECL> &constructor,
               const std::shared_ptr<AST_DESTRUCTOR_DECL> &destructor) noexcept
      -> std::expected<std::shared_ptr<AST_STRUCT>, Error>;

  [[nodiscard]] static auto createAttrAccess(const SourceLocation &srcLoc,
                                             const std::string &id) noexcept
      -> std::expected<std::shared_ptr<AST_ATTR_ACCESS>, Error>;

  [[nodiscard]] static auto
  createMethodCall(const SourceLocation &srcLoc, const std::string &id,
                   const std::vector<std::shared_ptr<Type>> &replaceOfGenerics,
                   const std::vector<std::shared_ptr<AST>> &parameters) noexcept
      -> std::expected<std::shared_ptr<AST_METHOD_CALL>, Error>;

  [[nodiscard]] static auto createMethodDecl(
      const SourceLocation &srcLoc, const std::string &id,
      const std::vector<GenericParameter> &generics, const Parameters &params,

      const std::shared_ptr<Type> &returnType,
      const std::shared_ptr<AST_BODY> &body, const bool isVirtual) noexcept
      -> std::expected<std::shared_ptr<AST_METHOD_DECL>, Error>;

  [[nodiscard]] static auto
  createConstructorDecl(const SourceLocation &srcLoc, const std::string &id,
                        const std::vector<GenericParameter> &generics,
                        const Parameters &params,
                        const std::shared_ptr<AST_SUPER> &super,
                        const std::shared_ptr<Type> &returnType,
                        const std::shared_ptr<AST_BODY> &body) noexcept
      -> std::expected<std::shared_ptr<AST_CONSTRUCTOR_DECL>, Error>;

  [[nodiscard]] static auto
  createDestructorDecl(const SourceLocation &srcLoc, const std::string &id,
                       const std::shared_ptr<Type> &returnType,
                       const std::shared_ptr<AST_BODY> &body,
                       const bool isVirtual) noexcept
      -> std::expected<std::shared_ptr<AST_DESTRUCTOR_DECL>, Error>;

  [[nodiscard]] static auto
  createSuper(const SourceLocation &srcLoc,
              const std::shared_ptr<Type> &fatherType,
              const std::vector<std::shared_ptr<Type>> &replacements,
              const std::vector<std::shared_ptr<AST>> &arguments) noexcept
      -> std::expected<std::shared_ptr<AST_SUPER>, Error>;

  [[nodiscard]] static auto createThis(const SourceLocation &srcLoc) noexcept
      -> std::expected<std::shared_ptr<AST_THIS>, Error>;

  [[nodiscard]] static auto createConstructorCall(
      const SourceLocation &srcLoc, const std::string &id,
      const std::vector<std::shared_ptr<Type>> &replaceOfGenerics,
      const std::vector<std::shared_ptr<AST>> &parameters) noexcept
      -> std::expected<std::shared_ptr<AST_CONSTRUCTOR_CALL>, Error>;

  // Variables
  [[nodiscard]] static auto
  createAutoDecl(const SourceLocation &srcLoc, const std::string &id,
                 const std::shared_ptr<AST> &value, const bool isConst) noexcept
      -> std::expected<std::shared_ptr<AST_AUTO_DECL>, Error>;

  [[nodiscard]] static auto
  createVarTypedtDecl(const SourceLocation &srcLoc, const std::string &id,
                      const std::shared_ptr<Type> &type,
                      const std::shared_ptr<AST> &value) noexcept
      -> std::expected<std::shared_ptr<AST_VAR_TYPED_DECL>, Error>;

  [[nodiscard]] static auto createVarCall(const SourceLocation &srcLoc,
                                          const std::string &id) noexcept
      -> std::expected<std::shared_ptr<AST_VAR_CALL>, Error>;

  // Chained expression
  [[nodiscard]] static auto
  createChained(const SourceLocation &srcLoc, const std::shared_ptr<AST> &base,
                const std::vector<std::shared_ptr<AST>> &operations) noexcept
      -> std::expected<std::shared_ptr<AST_CHAINED>, Error>;

  // Tree
  [[nodiscard]] static auto
  createTree(const std::shared_ptr<AST_BODY> &statements) noexcept
      -> std::expected<std::shared_ptr<Tree>, Error>;
};

} // namespace nicole

#endif