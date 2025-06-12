#include "../../../../inc/parsingAnalysis/algorithm/topDown.h"

namespace nicole {

auto 
TopDown::parseImport() const noexcept -> std::expected<std::shared_ptr<AST_IMPORT>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() != TokenType::STRING) {
    return createError(ERROR_TYPE::SINTAX, "missing file of import at " +
                                               tkStream_.current()->locInfo());
  }
  const Token token{*tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  std::filesystem::path currentFilePath{token.location().file()};
  // Gets the base path of the current file
  currentFilePath = currentFilePath.parent_path();
  // builds the path based on the import in the current file's directory
  currentFilePath =
      currentFilePath / token.raw().substr(1, token.raw().size() - 2);
  if (!parsedFiles_.count(currentFilePath)) {
    parsedFiles_.insert(currentFilePath);
    const std::expected<TokenStream, Error> tokens{
        lexer_.analyze(currentFilePath)};
    if (!tokens) {
      return createError(tokens.error());
    }
    const std::expected<std::monostate, Error> isInserted{
        tkStream_.insertAfter(*tokens, tkStream_.currentPos() + 1)};
    if (!isInserted) {
      return createError(isInserted.error());
    }
  }
  if (tkStream_.current()->type() != TokenType::SEMICOLON) {
    return createError(ERROR_TYPE::SINTAX, "missing ; of import statement at " +
                                               tkStream_.current()->locInfo());
  }
  return Builder::createImport(
      SourceLocation{*firsToken, *tkStream_.lastRead()}, currentFilePath);
}

auto 
TopDown::parsePrint() const noexcept -> std::expected<std::shared_ptr<AST_PRINT>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }

  const std::expected<std::vector<std::shared_ptr<AST>>, Error> arguemnts{
      parseArguments({TokenType::LP, TokenType::RP}, false)};
  if (!arguemnts) {
    return createError(arguemnts.error());
  }

  if (tkStream_.current()->type() != TokenType::SEMICOLON) {
    return createError(ERROR_TYPE::SINTAX, "missing ; of print statement at " +
                                               tkStream_.lastRead()->locInfo());
  }
  return Builder::createPrint(SourceLocation{*firsToken, *tkStream_.lastRead()},
                              *arguemnts);
}

} // namespace nicole