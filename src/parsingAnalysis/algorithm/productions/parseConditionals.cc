#include "../../../../inc/parsingAnalysis/algorithm/topDown.h"

namespace nicole {

auto TopDown::parseIf() const noexcept
    -> std::expected<std::shared_ptr<AST_IF>, Error> {
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
  const std::expected<std::shared_ptr<AST_BODY>, Error> bodyIf{parseBody()};
  if (!bodyIf || !*bodyIf) {
    return createError(bodyIf ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                              : bodyIf.error());
  }
  std::vector<std::shared_ptr<AST_ELSE_IF>> elseIfs{};
  while (tkStream_.currentPos() < tkStream_.size() and
         tkStream_.current()->type() == TokenType::ELSE and
         tkStream_.lookAhead(1)->type() == TokenType::IF) {
    const std::expected<std::shared_ptr<AST_ELSE_IF>, Error> elseIf{
        parseElseIf()};
    if (!elseIf || !*elseIf) {
      return createError(elseIf ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                                : elseIf.error());
    }
    elseIfs.push_back(*elseIf);
  }
  if (tkStream_.current()->type() != TokenType::ELSE) {
    return Builder::createIf(SourceLocation{*firsToken, *tkStream_.lastRead()},
                             *condition, *bodyIf, elseIfs, nullptr);
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  const std::expected<std::shared_ptr<AST_BODY>, Error> bodyElse{parseBody()};
  if (!bodyElse || !*bodyElse) {
    return createError(bodyElse ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                                : bodyElse.error());
  }
  return Builder::createIf(SourceLocation{*firsToken, *tkStream_.lastRead()},
                           *condition, *bodyIf, elseIfs, *bodyElse);
}

auto TopDown::parseElseIf() const noexcept
    -> std::expected<std::shared_ptr<AST_ELSE_IF>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
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
  const std::expected<std::shared_ptr<AST_BODY>, Error> body{parseBody()};
  if (!body || !*body) {
    return createError(body ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                            : body.error());
  }
  return Builder::createElseIf(
      SourceLocation{*firsToken, *tkStream_.lastRead()}, *condition, *body);
}

auto TopDown::parseSwitch() const noexcept
    -> std::expected<std::shared_ptr<AST_SWITCH>, Error> {
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
  if (tkStream_.current()->type() != TokenType::LB) {
    return createError(ERROR_TYPE::SINTAX,
                       "missing left bracket of switch at " +
                           tkStream_.current()->locInfo());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  std::vector<std::shared_ptr<AST_CASE>> cases{};
  while (tkStream_.currentPos() < tkStream_.size() and
         tkStream_.current()->type() == TokenType::CASE) {
    const std::expected<std::shared_ptr<AST_CASE>, Error> switchCase{
        parseCase()};
    if (!switchCase || !*switchCase) {
      return createError(switchCase
                             ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                             : switchCase.error());
    }
    cases.push_back(*switchCase);
  }
  if (!cases.size() and tkStream_.current()->type() == TokenType::RB) {
    return createError(ERROR_TYPE::SINTAX,
                       "missing at least one case or default at " +
                           tkStream_.current()->locInfo());
  }
  if (tkStream_.current()->type() == TokenType::RB) {
    if (std::expected<std::monostate, Error> res = tryEat(); !res) {
      return createError(res.error());
    }
    return Builder::createSwitch(
        SourceLocation{*firsToken, *tkStream_.lastRead()}, *condition, cases,
        nullptr);
  }
  if (tkStream_.current()->type() != TokenType::DEFAULT) {
    return createError(ERROR_TYPE::SINTAX,
                       "missing default case of switch at " +
                           tkStream_.current()->locInfo());
  }
  const std::expected<std::shared_ptr<AST_DEFAULT>, Error> defaultCase{
      parseDefault()};
  if (!defaultCase || !*defaultCase) {
    return createError(defaultCase
                           ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                           : defaultCase.error());
  }
  if (tkStream_.current()->type() != TokenType::RB) {
    return createError(ERROR_TYPE::SINTAX,
                       "missing right bracket of switch at " +
                           tkStream_.current()->locInfo());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  return Builder::createSwitch(
      SourceLocation{*firsToken, *tkStream_.lastRead()}, *condition, cases,
      *defaultCase);
}

auto TopDown::parseCase() const noexcept
    -> std::expected<std::shared_ptr<AST_CASE>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  const std::expected<std::shared_ptr<AST>, Error> condition{parseOr()};
  if (!condition || !*condition) {
    return createError(condition ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                                 : condition.error());
  }
  if (tkStream_.current()->type() != TokenType::DOTDOT) {
    return createError(ERROR_TYPE::SINTAX, "missing : of case at " +
                                               tkStream_.current()->locInfo());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  const std::expected<std::shared_ptr<AST_BODY>, Error> body{parseBody()};
  if (!body || !*body) {
    return createError(body ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                            : body.error());
  }
  return Builder::createCase(SourceLocation{*firsToken, *tkStream_.lastRead()},
                             *condition, *body);
}

auto TopDown::parseDefault() const noexcept
    -> std::expected<std::shared_ptr<AST_DEFAULT>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  const std::expected<std::shared_ptr<AST_BODY>, Error> body{parseBody()};
  if (!body || !*body) {
    return createError(body ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                            : body.error());
  }
  return Builder::createDefault(
      SourceLocation{*firsToken, *tkStream_.lastRead()}, *body);
}

auto TopDown::parseTernary() const noexcept
    -> std::expected<std::shared_ptr<AST>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  // to avoid ambigious situations like (something) being treated like ternary
  if (tkStream_.current()->type() == TokenType::LP and
      tkStream_.isTokenAheadBeforeSemicolon(TokenType::TERNARY)) {
    const std::expected<std::shared_ptr<AST_CONDITION>, Error> condition{
        parseCondition(false)};
    if (!condition || !*condition) {
      return createError(condition
                             ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                             : condition.error());
    }
    if (tkStream_.current()->type() != TokenType::TERNARY) {
      return createError(ERROR_TYPE::SINTAX,
                         "missing ? of ternary operator at " +
                             tkStream_.current()->locInfo());
    }
    if (std::expected<std::monostate, Error> res = tryEat(); !res) {
      return createError(res.error());
    }
    const std::expected<std::shared_ptr<AST>, Error> first{parseOr()};
    if (!first || !*first) {
      return createError(first ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                               : first.error());
    }
    if (tkStream_.current()->type() != TokenType::DOTDOT) {
      return createError(ERROR_TYPE::SINTAX,
                         "missing : of terna operator at " +
                             tkStream_.current()->locInfo());
    }
    if (std::expected<std::monostate, Error> res = tryEat(); !res) {
      return createError(res.error());
    }
    const std::expected<std::shared_ptr<AST>, Error> second{parseOr()};
    if (!second || !*second) {
      return createError(second ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                                : second.error());
    }
    return Builder::createTernary(
        SourceLocation{*firsToken, *tkStream_.lastRead()}, *condition, *first,
        *second);
  }
  return parseOr();
}

auto TopDown::parseCondition(const bool isInsideFor) const noexcept
    -> std::expected<std::shared_ptr<AST_CONDITION>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (!isInsideFor) {
    if (tkStream_.current()->type() != TokenType::LP) {
      return createError(ERROR_TYPE::SINTAX,
                         "missing left parenthesis at " +
                             tkStream_.current()->locInfo());
    }
    if (std::expected<std::monostate, Error> res = tryEat(); !res) {
      return createError(res.error());
    }
    if (tkStream_.current()->type() == TokenType::RP) {
      return createError(ERROR_TYPE::SINTAX,
                         "empty expression of condition at " +
                             tkStream_.current()->locInfo());
    }
  }
  const std::expected<std::shared_ptr<AST>, Error> condition{parseTernary()};
  if (!condition || !*condition) {
    return createError(condition ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                                 : condition.error());
  }
  if (!isInsideFor) {
    if (tkStream_.current()->type() != TokenType::RP) {
      return createError(ERROR_TYPE::SINTAX,
                         "missing right parenthesis of condition at " +
                             tkStream_.current()->locInfo());
    }
    if (std::expected<std::monostate, Error> res = tryEat(); !res) {
      return createError(res.error());
    }
  }
  return Builder::createCondition(
      SourceLocation{*firsToken, *tkStream_.lastRead()}, *condition);
}

} // namespace nicole