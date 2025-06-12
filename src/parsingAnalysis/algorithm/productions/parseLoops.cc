#include "../../../../inc/parsingAnalysis/algorithm/topDown.h"

namespace nicole {

auto TopDown::parseWhile() const noexcept
    -> std::expected<std::shared_ptr<AST_WHILE>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  const std::expected<std::shared_ptr<AST_CONDITION>, Error> condition{
      parseCondition(false)};
  if (!condition || !*condition) {
    return createError(condition ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                                 : condition.error());
  }
  const std::expected<std::shared_ptr<AST_BODY>, Error> body{parseBody()};
  if (!body || !*body) {
    return createError(body ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                            : body.error());
  }
  return Builder::createWhile(SourceLocation{*firsToken, *tkStream_.lastRead()},
                              *condition, *body);
}

auto TopDown::parseFor() const noexcept
    -> std::expected<std::shared_ptr<AST_FOR>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() != TokenType::LP) {
    return createError(ERROR_TYPE::SINTAX,
                       "missing left parenthesis of for at " +
                           tkStream_.current()->locInfo());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() == TokenType::RP) {
    return createError(ERROR_TYPE::SINTAX,
                       "empty for at " + tkStream_.current()->locInfo());
  }
  std::vector<std::shared_ptr<AST>> init{};
  while (tkStream_.currentPos() < tkStream_.size() and
         tkStream_.current()->type() != TokenType::SEMICOLON) {
    const std::expected<std::shared_ptr<AST>, Error> expression{
        parseVarDecl(true)};
    if (!expression || !*expression) {
      return createError(expression
                             ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                             : expression.error());
    }
    init.push_back(*expression);
    if (tkStream_.current()->type() == TokenType::COMMA) {
      if (std::expected<std::monostate, Error> res = tryEat(); !res) {
        return createError(res.error());
      }
      continue;
    } else if (tkStream_.current()->type() != TokenType::SEMICOLON) {
      return createError(ERROR_TYPE::SINTAX,
                         "missing comma or ; of FOR at " +
                             tkStream_.current()->locInfo());
    }
    break;
  }
  if (tkStream_.current()->type() != TokenType::SEMICOLON) {
    return createError(ERROR_TYPE::SINTAX, "missing ; after init of for at " +
                                               tkStream_.current()->locInfo());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }

  const std::expected<std::shared_ptr<AST_CONDITION>, Error> condition{
      (tkStream_.current()->type() == TokenType::SEMICOLON)
          ? *Builder::createCondition(
                SourceLocation{*firsToken, *tkStream_.lastRead()},
                *Builder::createBool(
                    SourceLocation{*firsToken, *tkStream_.lastRead()}, true))
          : parseCondition(true)};
  if (!condition || !*condition) {
    return createError(condition ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                                 : condition.error());
  }
  if (tkStream_.current()->type() != TokenType::SEMICOLON) {
    return createError(ERROR_TYPE::SINTAX,
                       "missing ; after condition of for at " +
                           tkStream_.current()->locInfo());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  std::vector<std::shared_ptr<AST>> update{};
  while (tkStream_.currentPos() < tkStream_.size() and
         tkStream_.current()->type() != TokenType::RP) {
    const std::expected<std::shared_ptr<AST>, Error> expression{
        parseAssignment(true)};
    if (!expression || !*expression) {
      return createError(expression
                             ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                             : expression.error());
    }
    update.push_back(*expression);
    if (tkStream_.current()->type() == TokenType::COMMA) {
      if (std::expected<std::monostate, Error> res = tryEat(); !res) {
        return createError(res.error());
      }
      continue;
    } else if (tkStream_.current()->type() != TokenType::RP) {
      return createError(ERROR_TYPE::SINTAX,
                         "missing comma or ) of for at " +
                             tkStream_.current()->locInfo());
    }
    break;
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  const std::expected<std::shared_ptr<AST_BODY>, Error> body{parseBody()};
  if (!body || !*body) {
    return createError(body ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                            : body.error());
  }
  return Builder::createFor(SourceLocation{*firsToken, *tkStream_.lastRead()},
                            init, *condition, update, *body);
}

auto TopDown::parseDoWhile() const noexcept
    -> std::expected<std::shared_ptr<AST_DO_WHILE>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  const std::expected<std::shared_ptr<AST_BODY>, Error> body{parseBody()};
  if (!body || !*body) {
    return createError(body ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                            : body.error());
  }
  if (tkStream_.current()->type() != TokenType::WHILE) {
    return createError(ERROR_TYPE::SINTAX,
                       "missing while keyword of do while at " +
                           tkStream_.current()->locInfo());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  const std::expected<std::shared_ptr<AST_CONDITION>, Error> condition{
      parseCondition(false)};
  if (!condition || !*condition) {
    return createError(condition ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                                 : condition.error());
  }
  if (!tkStream_.current() or
      tkStream_.current()->type() != TokenType::SEMICOLON) {
    return createError(ERROR_TYPE::SINTAX,
                       "missing ; of do while statement at " +
                           (*tkStream_.lastRead()).locInfo());
  }
  return Builder::createDoWhile(
      SourceLocation{*firsToken, *tkStream_.lastRead()}, *body, *condition);
}

auto TopDown::parsePass() const noexcept
    -> std::expected<std::shared_ptr<AST_PASS>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() != TokenType::SEMICOLON) {
    return createError(ERROR_TYPE::SINTAX, "missing ; of pass statement at " +
                                               tkStream_.current()->locInfo());
  }
  return Builder::createPass(SourceLocation{*firsToken, *tkStream_.lastRead()});
}

auto TopDown::parseStop() const noexcept
    -> std::expected<std::shared_ptr<AST_STOP>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() != TokenType::SEMICOLON) {
    return createError(ERROR_TYPE::SINTAX, "missing ; of stop statement at " +
                                               tkStream_.current()->locInfo());
  }
  return Builder::createStop(SourceLocation{*firsToken, *tkStream_.lastRead()});
}

} // namespace nicole