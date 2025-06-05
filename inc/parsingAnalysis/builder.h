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
  [[nodiscard]] static long long unsigned generateNextId() noexcept {
    static long long unsigned id{0};
    return ++id;
  }

  Builder() noexcept = delete;

public:
  // Literals
  [[nodiscard]] static std::expected<std::shared_ptr<AST_BOOL>, Error>
  createBool(const SourceLocation &srcLoc, const bool value) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_CHAR>, Error>
  createChar(const SourceLocation &srcLoc, const std::string &value) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_DOUBLE>, Error>
  createDouble(const SourceLocation &srcLoc, const double value) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_FLOAT>, Error>
  createFloat(const SourceLocation &srcLoc, const float value) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_INT>, Error>
  createInt(const SourceLocation &srcLoc, const int value) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_NULL>, Error>
  createNull(const SourceLocation &srcLoc) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_STRING>, Error>
  createString(const SourceLocation &srcLoc, const std::string value) noexcept;

  // Vectors
  [[nodiscard]] static std::expected<std::shared_ptr<AST_VECTOR>, Error>
  createVector(const SourceLocation &srcLoc,
               const std::vector<std::shared_ptr<AST>> values) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_INDEX>, Error>
  createIndex(const SourceLocation &srcLoc,
              const std::shared_ptr<AST> value) noexcept;

  // Pointers
  [[nodiscard]] static std::expected<std::shared_ptr<AST_DELETE>, Error>
  createDelete(const SourceLocation &srcLoc,
               const std::shared_ptr<AST> &value) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_NEW>, Error>
  createNew(const SourceLocation &srcLoc,
            const std::shared_ptr<AST> &value) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_DEREF>, Error>
  createDeref(const SourceLocation &srcLoc,
              const std::shared_ptr<AST> &value) noexcept;

  // Binary
  [[nodiscard]] static std::expected<std::shared_ptr<AST_BINARY>, Error>
  createBinary(const SourceLocation &srcLoc, const Token &op,
               const std::shared_ptr<AST> &left,
               const std::shared_ptr<AST> &right) noexcept;

  // Unary
  [[nodiscard]] static std::expected<std::shared_ptr<AST_UNARY>, Error>
  createUnary(const SourceLocation &srcLoc, const Token &op,
              const std::shared_ptr<AST> &value) noexcept;

  // Asignment

  [[nodiscard]] static std::expected<std::shared_ptr<AST_ASSIGNMENT>, Error>
  createAssignment(const SourceLocation &srcLoc, const Token &op,
                   const std::shared_ptr<AST> &left,
                   const std::shared_ptr<AST> &value) noexcept;

  // Utils
  [[nodiscard]] static std::expected<std::shared_ptr<AST_PRINT>, Error>
  createPrint(const SourceLocation &srcLoc,
              const std::vector<std::shared_ptr<AST>> &values) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_IMPORT>, Error>
  createImport(const SourceLocation &srcLoc,
               const std::filesystem::path &path) noexcept;

  // Statements
  [[nodiscard]] static std::expected<std::shared_ptr<AST_STATEMENT>, Error>
  createStatement(const SourceLocation &srcLoc,
                  const std::shared_ptr<AST> &expression) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_BODY>, Error>
  createBody(const SourceLocation &srcLoc,
             const std::vector<std::shared_ptr<AST_STATEMENT>> &body) noexcept;

  // Loops
  [[nodiscard]] static std::expected<std::shared_ptr<AST_WHILE>, Error>
  createWhile(const SourceLocation &srcLoc,
              const std::shared_ptr<AST_CONDITION> &condition,
              const std::shared_ptr<AST_BODY> &body) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_FOR>, Error>
  createFor(const SourceLocation &srcLoc,
            const std::vector<std::shared_ptr<AST>> &init,
            const std::shared_ptr<AST_CONDITION> &condition,
            const std::vector<std::shared_ptr<AST>> &update,
            const std::shared_ptr<AST_BODY> &body) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_DO_WHILE>, Error>
  createDoWhile(const SourceLocation &srcLoc,
                const std::shared_ptr<AST_BODY> &body,
                const std::shared_ptr<AST_CONDITION> &condition) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_PASS>, Error>
  createPass(const SourceLocation &srcLoc) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_STOP>, Error>
  createStop(const SourceLocation &srcLoc) noexcept;

  // Conditionals
  [[nodiscard]] static std::expected<std::shared_ptr<AST_IF>, Error>
  createIf(const SourceLocation &srcLoc,
           const std::shared_ptr<AST_CONDITION> &condition,
           const std::shared_ptr<AST_BODY> &body,
           const std::vector<std::shared_ptr<AST_ELSE_IF>> &elseIf,
           const std::shared_ptr<AST_BODY> &elseBody) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_ELSE_IF>, Error>
  createElseIf(const SourceLocation &srcLoc,
               const std::shared_ptr<AST_CONDITION> &condition,
               const std::shared_ptr<AST_BODY> &body) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_SWITCH>, Error>
  createSwitch(const SourceLocation &srcLoc,
               const std::shared_ptr<AST_CONDITION> &condition,
               const std::vector<std::shared_ptr<AST_CASE>> &cases,
               const std::shared_ptr<AST_DEFAULT> &default_) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_CASE>, Error>
  createCase(const SourceLocation &srcLoc, const std::shared_ptr<AST> &match,
             const std::shared_ptr<AST_BODY> &body) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_DEFAULT>, Error>
  createDefault(const SourceLocation &srcLoc,
                const std::shared_ptr<AST_BODY> &body) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_TERNARY>, Error>
  createTernary(const SourceLocation &srcLoc,
                const std::shared_ptr<AST_CONDITION> &condition,
                const std::shared_ptr<AST> &first,
                const std::shared_ptr<AST> &second) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_CONDITION>, Error>
  createCondition(const SourceLocation &srcLoc,
                  const std::shared_ptr<AST> &expression) noexcept;

  // Functions
  [[nodiscard]] static std::expected<std::shared_ptr<AST_FUNC_CALL>, Error>
  createFunCall(const SourceLocation &srcLoc, const std::string &id,
                const std::vector<std::shared_ptr<Type>> &replaceOfGenerics,
                const std::vector<std::shared_ptr<AST>> &parameters) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_FUNC_DECL>, Error>
  createFuncDecl(const SourceLocation &srcLoc, const std::string &id,
                 const std::vector<GenericParameter> &generics,
                 const Parameters &params,
                 const std::shared_ptr<Type> &returnType,
                 const std::shared_ptr<AST_BODY> &body) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_RETURN>, Error>
  createReturn(const SourceLocation &srcLoc,
               const std::shared_ptr<AST> &value) noexcept;

  // Enum
  [[nodiscard]] static std::expected<std::shared_ptr<AST_ENUM>, Error>
  createEnum(const SourceLocation &srcLoc, const std::string &id,
             const std::vector<std::string> &enumIdentifiers) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_ENUM_ACCESS>, Error>
  createEnumAccess(const SourceLocation &srcLoc, const std::string &id,
                   const std::string &identifiers) noexcept;

  // Usert types
  [[nodiscard]] static std::expected<std::shared_ptr<AST_STRUCT>, Error>
  createStruct(const SourceLocation &srcLoc, const std::string &id,
               const std::vector<GenericParameter> &generics,
               const std::shared_ptr<Type> &fatherType,
               const Attributes &attributes,
               const std::vector<std::shared_ptr<AST_METHOD_DECL>> &methods,
               const std::shared_ptr<AST_CONSTRUCTOR_DECL> &constructor,
               const std::shared_ptr<AST_DESTRUCTOR_DECL> &destructor) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_ATTR_ACCESS>, Error>
  createAttrAccess(const SourceLocation &srcLoc,
                   const std::string &id) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_METHOD_CALL>, Error>
  createMethodCall(
      const SourceLocation &srcLoc, const std::string &id,
      const std::vector<std::shared_ptr<Type>> &replaceOfGenerics,
      const std::vector<std::shared_ptr<AST>> &parameters) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_METHOD_DECL>, Error>
  createMethodDecl(const SourceLocation &srcLoc, const std::string &id,
                   const std::vector<GenericParameter> &generics,
                   const Parameters &params,

                   const std::shared_ptr<Type> &returnType,
                   const std::shared_ptr<AST_BODY> &body,
                   const bool isVirtual) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_CONSTRUCTOR_DECL>,
                                     Error>
  createConstructorDecl(const SourceLocation &srcLoc, const std::string &id,
                        const std::vector<GenericParameter> &generics,
                        const Parameters &params,
                        const std::shared_ptr<AST_SUPER> &super,
                        const std::shared_ptr<Type> &returnType,
                        const std::shared_ptr<AST_BODY> &body) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_DESTRUCTOR_DECL>,
                                     Error>
  createDestructorDecl(const SourceLocation &srcLoc, const std::string &id,
                       const std::shared_ptr<Type> &returnType,
                       const std::shared_ptr<AST_BODY> &body,
                       const bool isVirtual) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_SUPER>, Error>
  createSuper(const SourceLocation &srcLoc,
              const std::shared_ptr<Type> &fatherType,
              const std::vector<std::shared_ptr<Type>> &replacements,
              const std::vector<std::shared_ptr<AST>> &arguments) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_THIS>, Error>
  createThis(const SourceLocation &srcLoc) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_CONSTRUCTOR_CALL>,
                                     Error>
  createConstructorCall(
      const SourceLocation &srcLoc, const std::string &id,
      const std::vector<std::shared_ptr<Type>> &replaceOfGenerics,
      const std::vector<std::shared_ptr<AST>> &parameters) noexcept;

  // Variables
  [[nodiscard]] static std::expected<std::shared_ptr<AST_AUTO_DECL>, Error>
  createAutoDecl(const SourceLocation &srcLoc, const std::string &id,
                 const std::shared_ptr<AST> &value,
                 const bool isConst) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_VAR_TYPED_DECL>, Error>
  createVarTypedtDecl(const SourceLocation &srcLoc, const std::string &id,
                      const std::shared_ptr<Type> &type,
                      const std::shared_ptr<AST> &value) noexcept;

  [[nodiscard]] static std::expected<std::shared_ptr<AST_VAR_CALL>, Error>
  createVarCall(const SourceLocation &srcLoc, const std::string &id) noexcept;

  // Chained expression
  [[nodiscard]] static std::expected<std::shared_ptr<AST_CHAINED>, Error>
  createChained(const SourceLocation &srcLoc, const std::shared_ptr<AST> &base,
                const std::vector<std::shared_ptr<AST>> &operations) noexcept;

  // Tree
  [[nodiscard]] static std::expected<std::shared_ptr<Tree>, Error>
  createTree(const std::shared_ptr<AST_BODY> &statements) noexcept;
};

} // namespace nicole

#endif