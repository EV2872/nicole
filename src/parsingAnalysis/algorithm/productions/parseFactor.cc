#include "../../../../inc/parsingAnalysis/algorithm/topDown.h"

namespace nicole {

auto TopDown::parseFactor() const noexcept
    -> std::expected<std::shared_ptr<AST>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  const std::string raw{tkStream_.current()->raw()};
  const std::string loc{tkStream_.current()->locInfo()};
  switch (tkStream_.current()->type()) {

  case TokenType::THIS: {
    if (!tkStream_.eat()) {
      break;
    }
    return Builder::createThis(
        SourceLocation{*firsToken, *tkStream_.lastRead()});
  }

  case TokenType::TRUE: {
    if (!tkStream_.eat()) {
      break;
    }
    return Builder::createBool(
        SourceLocation{*firsToken, *tkStream_.lastRead()}, true);
  }

  case TokenType::FALSE: {
    if (!tkStream_.eat()) {
      break;
    }
    return Builder::createBool(
        SourceLocation{*firsToken, *tkStream_.lastRead()}, false);
  }

  case TokenType::NUMBER_INT: {
    const int value{std::stoi(raw)};
    if (!tkStream_.eat()) {
      break;
    }
    return Builder::createInt(SourceLocation{*firsToken, *tkStream_.lastRead()},
                              value);
  }

  case TokenType::NUMBER_DOUBLE: {
    const double value{std::stod(raw)};
    if (!tkStream_.eat()) {
      break;
    }
    return Builder::createDouble(
        SourceLocation{*firsToken, *tkStream_.lastRead()}, value);
  }

  case TokenType::NUMBER_FLOAT: {
    const float value{std::stof(raw.substr(1))};
    if (!tkStream_.eat()) {
      break;
    }
    return Builder::createFloat(
        SourceLocation{*firsToken, *tkStream_.lastRead()}, value);
  }

  case TokenType::STRING: {
    const std::string value{raw};
    if (!tkStream_.eat()) {
      break;
    }
    return Builder::createString(
        SourceLocation{*firsToken, *tkStream_.lastRead()}, value);
  }

  case TokenType::CHAR: {
    const std::string value{raw};
    if (!tkStream_.eat()) {
      break;
    }
    return Builder::createChar(
        SourceLocation{*firsToken, *tkStream_.lastRead()}, value);
  }

  case TokenType::NULLPTR: {
    if (!tkStream_.eat()) {
      break;
    }
    return Builder::createNull(
        SourceLocation{*firsToken, *tkStream_.lastRead()});
  }

  case TokenType::ID: {
    if (tkStream_.lookAhead(1)->type() == TokenType::DOTDOT and
        tkStream_.lookAhead(2)->type() == TokenType::DOTDOT) {
      return parseEnumAccess();
    }
    return parseChainedExpression();
  }

  case TokenType::LC: {
    return parseVector();
  }

  case TokenType::LP: {
    if (!tkStream_.eat()) {
      break;
    }
    if (tkStream_.current()->type() == TokenType::RP) {
      return createError(ERROR_TYPE::SINTAX, "empty expression at " + loc);
    }
    const std::expected<std::shared_ptr<AST>, Error> expression{parseOr()};
    if (!expression || !*expression) {
      return createError(expression
                             ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                             : expression.error());
    }
    if (tkStream_.current()->type() != TokenType::RP) {
      return createError(ERROR_TYPE::SINTAX,
                         "missing right parenthesis at " +
                             tkStream_.current()->locInfo());
    }
    if (!tkStream_.eat()) {
      break;
    }
    return expression;
  }

  case TokenType::OPERATOR_SUB: {
    const Token token{*tkStream_.current()};
    if (!tkStream_.eat()) {
      break;
    }
    const std::expected<std::shared_ptr<AST>, Error> expression{parseOr()};
    if (!expression || !*expression) {
      return createError(expression
                             ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                             : expression.error());
    }
    return Builder::createUnary(
        SourceLocation{*firsToken, *tkStream_.lastRead()}, token, *expression);
  }

  case TokenType::OPERATOR_NOT: {
    const Token token{*tkStream_.current()};
    if (!tkStream_.eat()) {
      break;
    }
    const std::expected<std::shared_ptr<AST>, Error> expression{parseOr()};
    if (!expression || !*expression) {
      return createError(expression
                             ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                             : expression.error());
    }
    return Builder::createUnary(
        SourceLocation{*firsToken, *tkStream_.lastRead()}, token, *expression);
  }

  case TokenType::DECREMENT: {
    const Token token{*tkStream_.current()};
    if (!tkStream_.eat()) {
      break;
    }
    const std::expected<std::shared_ptr<AST>, Error> expression{parseOr()};
    if (!expression || !*expression) {
      return createError(expression
                             ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                             : expression.error());
    }
    return Builder::createUnary(
        SourceLocation{*firsToken, *tkStream_.lastRead()}, token, *expression);
  }

  case TokenType::INCREMENT: {
    const Token token{*tkStream_.current()};
    if (!tkStream_.eat()) {
      break;
    }
    const std::expected<std::shared_ptr<AST>, Error> expression{parseOr()};
    if (!expression || !*expression) {
      return createError(expression
                             ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                             : expression.error());
    }
    return Builder::createUnary(
        SourceLocation{*firsToken, *tkStream_.lastRead()}, token, *expression);
  }

  case TokenType::NEW: {
    const Token token{*tkStream_.current()};
    if (!tkStream_.eat()) {
      break;
    }
    const std::expected<std::shared_ptr<AST>, Error> expression{parseOr()};
    if (!expression || !*expression) {
      return createError(expression
                             ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                             : expression.error());
    }
    return Builder::createNew(SourceLocation{*firsToken, *tkStream_.lastRead()},
                              *expression);
  }

  case TokenType::OPERATOR_MULT: {
    if (!tkStream_.eat()) {
      break;
    }
    const std::expected<std::shared_ptr<AST>, Error> expression{parseOr()};
    if (!expression || !*expression) {
      return createError(expression
                             ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                             : expression.error());
    }
    return Builder::createDeref(
        SourceLocation{*firsToken, *tkStream_.lastRead()}, *expression);
  }

  default:
    // error
    return createError(ERROR_TYPE::SINTAX,
                       "token " + raw + " is in a wrong position at " + loc);
  }
  return createError(ERROR_TYPE::SINTAX, "failed to eat " + raw + " at " + loc);
}

auto TopDown::parseVector() const noexcept
    -> std::expected<std::shared_ptr<AST_VECTOR>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  const std::expected<std::vector<std::shared_ptr<AST>>, Error> arguemnts{
      parseArguments({TokenType::LC, TokenType::RC}, true)};
  if (!arguemnts) {
    return createError(arguemnts.error());
  }
  return Builder::createVector(
      SourceLocation{*firsToken, *tkStream_.lastRead()}, *arguemnts);
}

auto TopDown::parseArguments(std::pair<TokenType, TokenType> delimiters,
                             const bool canBeEmpty) const noexcept
    -> std::expected<std::vector<std::shared_ptr<AST>>, Error> {
  if (tkStream_.current()->type() != delimiters.first) {
    return createError(ERROR_TYPE::SINTAX,
                       "missing " + tokenTypeToString(delimiters.first) +
                           " at " + tkStream_.current()->locInfo());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() == delimiters.second and !canBeEmpty) {
    return createError(ERROR_TYPE::SINTAX,
                       "empty expression at " + tkStream_.current()->locInfo());
  }
  std::vector<std::shared_ptr<AST>> params{};
  while (tkStream_.currentPos() < tkStream_.size() and
         tkStream_.current()->type() != delimiters.second) {
    const std::expected<std::shared_ptr<AST>, Error> param{parseTernary()};
    if (!param || !*param) {
      return createError(param ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                               : param.error());
    }
    params.push_back(*param);
    if (tkStream_.current()->type() == TokenType::COMMA) {
      if (std::expected<std::monostate, Error> res = tryEat(); !res) {
        return createError(res.error());
      }
      continue;
    } else if (tkStream_.current()->type() != delimiters.second) {
      return createError(ERROR_TYPE::SINTAX,
                         "missing " + tokenTypeToString(delimiters.second) +
                             " at " + tkStream_.current()->locInfo() +
                             " found: " + tkStream_.current()->raw());
    }
    break;
  }
  if (tkStream_.current()->type() != delimiters.second) {
    return createError(ERROR_TYPE::SINTAX,
                       "missing " + tokenTypeToString(delimiters.second) +
                           " at " + tkStream_.current()->locInfo() +
                           " found: " + tkStream_.current()->raw());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  return params;
}

} // namespace nicole