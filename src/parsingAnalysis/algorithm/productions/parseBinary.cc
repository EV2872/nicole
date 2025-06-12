#include "../../../../inc/parsingAnalysis/algorithm/topDown.h"

namespace nicole {

auto TopDown::parseOr() const noexcept
    -> std::expected<std::shared_ptr<AST>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  std::expected<std::shared_ptr<AST>, Error> left{parseAnd()};

  if (!left || !*left) {
    return std::unexpected{left ? Error{ERROR_TYPE::NULL_NODE, "left is null"}
                                : left.error()};
  }

  while (tkStream_.currentPos() < tkStream_.size() &&
         (tkStream_.current()->type() == TokenType::OR)) {
    const Token token{*tkStream_.current()};
    std::expected<std::monostate, Error> eaten{tkStream_.eat()};
    if (!eaten) {
      return std::unexpected{
          Error{ERROR_TYPE::SINTAX,
                "failed to eat " + token.raw() + " at " + token.locInfo()}};
    }

    const std::expected<std::shared_ptr<AST>, Error> right{parseAnd()};
    if (!right || !*right) {
      return std::unexpected{right
                                 ? Error{ERROR_TYPE::NULL_NODE,
                                         "right is null at " + token.locInfo()}
                                 : right.error()};
    }

    left =
        Builder::createBinary(SourceLocation{*firsToken, *tkStream_.lastRead()},
                              token, *left, *right);
    if (!left) {
      return std::unexpected{left.error()};
    }
  }

  return left;
}

auto TopDown::parseAnd() const noexcept
    -> std::expected<std::shared_ptr<AST>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  std::expected<std::shared_ptr<AST>, Error> left{parseEqual_NotEqual()};

  if (!left || !*left) {
    return std::unexpected{left ? Error{ERROR_TYPE::NULL_NODE, "left is null"}
                                : left.error()};
  }

  while (tkStream_.currentPos() < tkStream_.size() &&
         (tkStream_.current()->type() == TokenType::AND)) {
    const Token token{*tkStream_.current()};
    std::expected<std::monostate, Error> eaten{tkStream_.eat()};
    if (!eaten) {
      return createError(ERROR_TYPE::SINTAX, "failed to eat " + token.raw() +
                                                 " at " + token.locInfo());
    }

    const std::expected<std::shared_ptr<AST>, Error> right{
        parseEqual_NotEqual()};
    if (!right || !*right) {
      return std::unexpected{right
                                 ? Error{ERROR_TYPE::NULL_NODE,
                                         "right is null at " + token.locInfo()}
                                 : right.error()};
    }

    left =
        Builder::createBinary(SourceLocation{*firsToken, *tkStream_.lastRead()},
                              token, *left, *right);
    if (!left) {
      return createError(left.error());
    }
  }

  return left;
}

auto TopDown::parseEqual_NotEqual() const noexcept
    -> std::expected<std::shared_ptr<AST>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  std::expected<std::shared_ptr<AST>, Error> left{parseCompare()};

  if (!left || !*left) {
    return std::unexpected{left ? Error{ERROR_TYPE::NULL_NODE, "left is null"}
                                : left.error()};
  }

  while (tkStream_.currentPos() < tkStream_.size() &&
         (tkStream_.current()->type() == TokenType::EQUAL ||
          tkStream_.current()->type() == TokenType::NOTEQUAL)) {
    const Token token{*tkStream_.current()};
    std::expected<std::monostate, Error> eaten{tkStream_.eat()};
    if (!eaten) {
      return createError(ERROR_TYPE::SINTAX, "failed to eat " + token.raw() +
                                                 " at " + token.locInfo());
    }

    const std::expected<std::shared_ptr<AST>, Error> right{parseCompare()};
    if (!right || !*right) {
      return std::unexpected{right
                                 ? Error{ERROR_TYPE::NULL_NODE,
                                         "right is null at " + token.locInfo()}
                                 : right.error()};
    }

    left =
        Builder::createBinary(SourceLocation{*firsToken, *tkStream_.lastRead()},
                              token, *left, *right);
    if (!left) {
      return createError(left.error());
    }
  }

  return left;
}

auto TopDown::parseCompare() const noexcept
    -> std::expected<std::shared_ptr<AST>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  std::expected<std::shared_ptr<AST>, Error> left{parseAdd_Sub()};

  if (!left || !*left) {
    return std::unexpected{left ? Error{ERROR_TYPE::NULL_NODE, "left is null"}
                                : left.error()};
  }

  while (tkStream_.currentPos() < tkStream_.size() &&
         (tkStream_.current()->type() == TokenType::OPERATOR_SMALLER ||
          tkStream_.current()->type() == TokenType::SMALLEREQUAL ||
          tkStream_.current()->type() == TokenType::OPERATOR_GREATER ||
          tkStream_.current()->type() == TokenType::BIGGEREQUAL)) {
    const Token token{*tkStream_.current()};
    std::expected<std::monostate, Error> eaten{tkStream_.eat()};
    if (!eaten) {
      return createError(ERROR_TYPE::SINTAX, "failed to eat " + token.raw() +
                                                 " at " + token.locInfo());
    }

    const std::expected<std::shared_ptr<AST>, Error> right{parseAdd_Sub()};
    if (!right || !*right) {
      return std::unexpected{right
                                 ? Error{ERROR_TYPE::NULL_NODE,
                                         "right is null at " + token.locInfo()}
                                 : right.error()};
    }

    left =
        Builder::createBinary(SourceLocation{*firsToken, *tkStream_.lastRead()},
                              token, *left, *right);
    if (!left) {
      return createError(left.error());
    }
  }

  return left;
}

auto TopDown::parseAdd_Sub() const noexcept
    -> std::expected<std::shared_ptr<AST>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  std::expected<std::shared_ptr<AST>, Error> left{parseMult_Div_Module()};

  if (!left || !*left) {
    return std::unexpected{left ? Error{ERROR_TYPE::NULL_NODE, "left is null"}
                                : left.error()};
  }

  while (tkStream_.currentPos() < tkStream_.size() &&
         (tkStream_.current()->type() == TokenType::OPERATOR_ADD ||
          tkStream_.current()->type() == TokenType::OPERATOR_SUB)) {
    const Token token{*tkStream_.current()};
    std::expected<std::monostate, Error> eaten{tkStream_.eat()};
    if (!eaten) {
      return createError(ERROR_TYPE::SINTAX, "failed to eat " + token.raw() +
                                                 " at " + token.locInfo());
    }

    const std::expected<std::shared_ptr<AST>, Error> right{
        parseMult_Div_Module()};
    if (!right || !*right) {
      return std::unexpected{right
                                 ? Error{ERROR_TYPE::NULL_NODE,
                                         "right is null at " + token.locInfo()}
                                 : right.error()};
    }

    left =
        Builder::createBinary(SourceLocation{*firsToken, *tkStream_.lastRead()},
                              token, *left, *right);
    if (!left) {
      return createError(left.error());
    }
  }

  return left;
}

auto TopDown::parseMult_Div_Module() const noexcept
    -> std::expected<std::shared_ptr<AST>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  std::expected<std::shared_ptr<AST>, Error> left{parseFactor()};

  if (!left || !*left) {
    return std::unexpected{left ? Error{ERROR_TYPE::NULL_NODE, "left is null"}
                                : left.error()};
  }

  while (tkStream_.currentPos() < tkStream_.size() &&
         (tkStream_.current()->type() == TokenType::OPERATOR_MULT ||
          tkStream_.current()->type() == TokenType::OPERATOR_DIV ||
          tkStream_.current()->type() == TokenType::OPERATOR_MODULE)) {
    const Token token{*tkStream_.current()};
    std::expected<std::monostate, Error> eaten{tkStream_.eat()};
    if (!eaten) {
      return createError(ERROR_TYPE::SINTAX, "failed to eat " + token.raw() +
                                                 " at " + token.locInfo());
    }

    const std::expected<std::shared_ptr<AST>, Error> right{parseFactor()};
    if (!right || !*right) {
      return std::unexpected{right
                                 ? Error{ERROR_TYPE::NULL_NODE,
                                         "right is null at " + token.locInfo()}
                                 : right.error()};
    }

    left =
        Builder::createBinary(SourceLocation{*firsToken, *tkStream_.lastRead()},
                              token, *left, *right);
    if (!left) {
      return createError(left.error());
    }
  }

  return left;
}

} // namespace nicole