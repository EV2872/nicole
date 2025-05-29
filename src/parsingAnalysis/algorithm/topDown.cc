#include "../../../inc/parsingAnalysis/algorithm/topDown.h"
#include <memory>

namespace nicole {

const std::expected<std::shared_ptr<Tree>, Error>
TopDown::parse(const std::filesystem::path &entryFile) const noexcept {
  parsedFiles_.insert(entryFile);

  const std::expected<TokenStream, Error> tkStream{lexer_.analyze(entryFile)};

  if (!tkStream) {
    return createError(tkStream.error());
  }

  if (!tkStream->size()) {
    return createError(ERROR_TYPE::SINTAX, "empty file");
  }

  tkStream_ = *tkStream;

  const std::expected<std::shared_ptr<AST_BODY>, Error> root{parseStart()};

  if (!root || !*root) {
    return createError(root ? Error{ERROR_TYPE::NULL_NODE, "Tree is null"}
                            : root.error());
  }

  const std::expected<std::shared_ptr<Tree>, Error> tree{
      Builder::createTree(*root)};

  if (!tree) {
    return createError(tree.error());
  }

  return *tree;
}

const std::expected<std::shared_ptr<AST_BODY>, Error>
TopDown::parseStart() const noexcept {
  std::vector<std::shared_ptr<AST_STATEMENT>> statements{};

  const std::expected<Token, Error> firsToken{tkStream_.current()};

  while (tkStream_.currentPos() < tkStream_.size()) {
    const std::expected<std::shared_ptr<AST_STATEMENT>, Error> statement{
        parseStatement()};
    if (!statement || !*statement) {
      return createError(statement
                             ? Error{ERROR_TYPE::NULL_NODE, "Statement is null"}
                             : statement.error());
    }
    statements.push_back(*statement);
    if (tkStream_.current()->type() == TokenType::SEMICOLON &&
        !tkStream_.eat()) {
      return createError(ERROR_TYPE::SINTAX, "Failed to consume semicolon");
    }
  }

  const std::expected<std::shared_ptr<AST_BODY>, Error> body{
      Builder::createBody(SourceLocation{*firsToken, *tkStream_.lastRead()},
                          statements)};
  if (!body) {
    return createError(body.error());
  }

  return body;
}

const std::expected<std::shared_ptr<AST_BODY>, Error>
TopDown::parseBody() const noexcept {

  const std::expected<Token, Error> firsToken{tkStream_.current()};

  if (tkStream_.current()->type() != TokenType::LB) {
    return createError(ERROR_TYPE::SINTAX,
                       "missing { at " + tkStream_.current()->locInfo());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }

  std::vector<std::shared_ptr<AST_STATEMENT>> statements{};

  while (tkStream_.currentPos() < tkStream_.size() and
         tkStream_.current()->type() != TokenType::RB) {
    const std::expected<std::shared_ptr<AST_STATEMENT>, Error> statement{
        parseStatement()};
    if (!statement || !*statement) {
      return createError(statement
                             ? Error{ERROR_TYPE::NULL_NODE, "Statement is null"}
                             : statement.error());
    }
    statements.push_back(*statement);
    if (tkStream_.current()->type() == TokenType::SEMICOLON &&
        !tkStream_.eat()) {
      return createError(ERROR_TYPE::SINTAX, "Failed to consume semicolon");
    }
  }
  if (tkStream_.current()->type() != TokenType::RB) {
    return createError(ERROR_TYPE::SINTAX,
                       "missing } at " + tkStream_.current()->locInfo());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }

  const std::expected<std::shared_ptr<AST_BODY>, Error> body{
      Builder::createBody(SourceLocation{*firsToken, *tkStream_.lastRead()},
                          statements)};
  if (!body) {
    return createError(body.error());
  }

  return body;
}

const std::expected<std::shared_ptr<AST_STATEMENT>, Error>
TopDown::parseStatement() const noexcept {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  // Función auxiliar para elegir la sentencia a parsear.
  auto parseStmt = [&]() -> std::expected<std::shared_ptr<AST>, Error> {
    // in case that it is an assignment
    if ((tkStream_.isTokenAheadBeforeSemicolon(TokenType::DOTDOT_ASSIGNMENT) ||
        tkStream_.isTokenAheadBeforeSemicolon(TokenType::SELF_ADD) ||
        tkStream_.isTokenAheadBeforeSemicolon(TokenType::SELF_SUB) ||
        tkStream_.isTokenAheadBeforeSemicolon(TokenType::SELF_MULT) ||
        tkStream_.isTokenAheadBeforeSemicolon(TokenType::SELF_DIV)) and (firsToken->type() != TokenType::STRUCT)  and (firsToken->type() != TokenType::FUNCTION))
      return parseAssignment(false);

    // Se obtiene el tipo del token actual para decidir la rama de parseo.
    switch (tkStream_.current()->type()) {
    case TokenType::IF:
      return parseIf();
    case TokenType::SWITCH:
      return parseSwitch();
    case TokenType::WHILE:
      return parseWhile();
    case TokenType::DO:
      return parseDoWhile();
    case TokenType::FOR:
      return parseFor();
    case TokenType::PASS:
      return parsePass();
    case TokenType::STOP:
      return parseStop();
    case TokenType::FUNCTION:
      return parseFuncDecl();
    case TokenType::RETURN:
      return parseReturn();
    case TokenType::ENUM:
      return parseEnum();
    case TokenType::STRUCT:
      return parseStructDecl();
    case TokenType::DELETE:
      return parseDelete();
    case TokenType::IMPORT:
      return parseImport();
    case TokenType::PRINT:
      return parsePrint();
    default:
      return parseVarDecl(false);
    }
  };

  std::expected<std::shared_ptr<AST>, Error> statement = parseStmt();

  if (!statement || !*statement)
    return createError(statement ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                                 : statement.error());

  return Builder::createStatement(
      SourceLocation{*firsToken, *tkStream_.lastRead()}, *statement);
}

// topDown.cpp
std::expected<std::monostate, Error> TopDown::tryEat() const noexcept {
  if (!tkStream_.eat())
    return createError(ERROR_TYPE::SINTAX,
                       "failed to eat " + tkStream_.current()->raw() + " at " +
                           tkStream_.current()->locInfo());
  return {};
}

} // namespace nicole