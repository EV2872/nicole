#ifndef TOP_DOWN_H
#define TOP_DOWN_H

#include "../../tables/typeTable/typeTable.h"
#include "../builder.h"
#include "parser.h"
#include <expected>
#include <memory>
#include <variant>

namespace nicole {

class TopDown final : public Parser {
private:
  [[nodiscard]] auto parseStart() const noexcept
      -> std::expected<std::shared_ptr<AST_BODY>, Error>;

  [[nodiscard]] auto parseType() const noexcept
      -> std::expected<std::shared_ptr<Type>, Error>;

  [[nodiscard]] auto parsePrimaryType() const noexcept
      -> std::expected<std::shared_ptr<Type>, Error>;

  [[nodiscard]] auto parseGenerics() const noexcept
      -> std::expected<std::vector<GenericParameter>, Error>;

  [[nodiscard]] auto parseReplacementOfGenerics() const noexcept
      -> std::expected<std::vector<std::shared_ptr<Type>>, Error>;

  [[nodiscard]] auto parseBody() const noexcept
      -> std::expected<std::shared_ptr<AST_BODY>, Error>;

  [[nodiscard]] auto parseStatement() const noexcept
      -> std::expected<std::shared_ptr<AST_STATEMENT>, Error>;

  [[nodiscard]] auto parseEnum() const noexcept
      -> std::expected<std::shared_ptr<AST_ENUM>, Error>;

  [[nodiscard]] auto parseEnumAccess() const noexcept
      -> std::expected<std::shared_ptr<AST_ENUM_ACCESS>, Error>;

  [[nodiscard]] auto parseStructDecl() const noexcept
      -> std::expected<std::shared_ptr<AST_STRUCT>, Error>;

  [[nodiscard]] auto parseConstructorDecl(
      const std::string &id_returnType, const std::shared_ptr<Type> &fatherType,
      const std::vector<GenericParameter> &classGenerics) const noexcept
      -> std::expected<std::shared_ptr<AST_CONSTRUCTOR_DECL>, Error>;

  [[nodiscard]] auto parseDestructorDecl(const std::string &id,
                                         const bool isVirtual) const noexcept
      -> std::expected<std::shared_ptr<AST_DESTRUCTOR_DECL>, Error>;

  [[nodiscard]] auto parseMethodDecl(const bool isVirtual) const noexcept
      -> std::expected<std::shared_ptr<AST_METHOD_DECL>, Error>;

  [[nodiscard]] auto parseDelete() const noexcept
      -> std::expected<std::shared_ptr<AST_DELETE>, Error>;

  [[nodiscard]] auto parseFuncDecl() const noexcept
      -> std::expected<std::shared_ptr<AST_FUNC_DECL>, Error>;

  [[nodiscard]] auto parseReturn() const noexcept
      -> std::expected<std::shared_ptr<AST_RETURN>, Error>;

  [[nodiscard]] auto parseImport() const noexcept
      -> std::expected<std::shared_ptr<AST_IMPORT>, Error>;

  [[nodiscard]] auto parsePrint() const noexcept
      -> std::expected<std::shared_ptr<AST_PRINT>, Error>;

  [[nodiscard]] auto parseWhile() const noexcept
      -> std::expected<std::shared_ptr<AST_WHILE>, Error>;

  [[nodiscard]] auto parseFor() const noexcept
      -> std::expected<std::shared_ptr<AST_FOR>, Error>;

  [[nodiscard]] auto parsePass() const noexcept
      -> std::expected<std::shared_ptr<AST_PASS>, Error>;

  [[nodiscard]] auto parseStop() const noexcept
      -> std::expected<std::shared_ptr<AST_STOP>, Error>;

  [[nodiscard]] auto parseDoWhile() const noexcept
      -> std::expected<std::shared_ptr<AST_DO_WHILE>, Error>;

  [[nodiscard]] auto parseIf() const noexcept
      -> std::expected<std::shared_ptr<AST_IF>, Error>;

  [[nodiscard]] auto parseElseIf() const noexcept
      -> std::expected<std::shared_ptr<AST_ELSE_IF>, Error>;

  [[nodiscard]] auto parseSwitch() const noexcept
      -> std::expected<std::shared_ptr<AST_SWITCH>, Error>;

  [[nodiscard]] auto parseCase() const noexcept
      -> std::expected<std::shared_ptr<AST_CASE>, Error>;

  [[nodiscard]] auto parseDefault() const noexcept
      -> std::expected<std::shared_ptr<AST_DEFAULT>, Error>;

  [[nodiscard]] auto parseAssignment(const bool insideFor) const noexcept
      -> std::expected<std::shared_ptr<AST>, Error>;

  [[nodiscard]] auto parseVarDecl(const bool insideFor) const noexcept
      -> std::expected<std::shared_ptr<AST>, Error>;

  [[nodiscard]] auto parseTernary() const noexcept
      -> std::expected<std::shared_ptr<AST>, Error>;

  [[nodiscard]] auto parseCondition(const bool isInsideFor) const noexcept
      -> std::expected<std::shared_ptr<AST_CONDITION>, Error>;

  [[nodiscard]] auto parseOr() const noexcept
      -> std::expected<std::shared_ptr<AST>, Error>;

  [[nodiscard]] auto parseAnd() const noexcept
      -> std::expected<std::shared_ptr<AST>, Error>;

  [[nodiscard]] auto parseEqual_NotEqual() const noexcept
      -> std::expected<std::shared_ptr<AST>, Error>;

  [[nodiscard]] auto parseCompare() const noexcept
      -> std::expected<std::shared_ptr<AST>, Error>;

  [[nodiscard]] auto parseAdd_Sub() const noexcept
      -> std::expected<std::shared_ptr<AST>, Error>;

  [[nodiscard]] auto parseMult_Div_Module() const noexcept
      -> std::expected<std::shared_ptr<AST>, Error>;

  [[nodiscard]] auto parseFactor() const noexcept
      -> std::expected<std::shared_ptr<AST>, Error>;

  [[nodiscard]] auto parseVector() const noexcept
      -> std::expected<std::shared_ptr<AST_VECTOR>, Error>;

  [[nodiscard]] auto parseChainedExpression() const noexcept
      -> std::expected<std::shared_ptr<AST_CHAINED>, Error>;

  [[nodiscard]] auto parseParams() const noexcept
      -> std::expected<Parameters, Error>;

  [[nodiscard]] auto parseArguments(std::pair<TokenType, TokenType> delimiters,
                                    const bool canBeEmpty) const noexcept
      -> std::expected<std::vector<std::shared_ptr<AST>>, Error>;

  [[nodiscard]] auto tryEat() const noexcept
      -> std::expected<std::monostate, Error>;

public:
  explicit TopDown(const std::shared_ptr<Sintax> &sintax) noexcept
      : Parser{sintax} {}

  [[nodiscard]] auto
  parse(const std::filesystem::path &entryFile) const noexcept
      -> std::expected<std::shared_ptr<Tree>, Error> override;
};

} // namespace nicole

#endif