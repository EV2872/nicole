#include "../../../../inc/parsingAnalysis/algorithm/topDown.h"
#include <memory>
#include <vector>

namespace nicole {

auto TopDown::parseFuncDecl() const noexcept
    -> std::expected<std::shared_ptr<AST_FUNC_DECL>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() != TokenType::ID) {
    return createError(ERROR_TYPE::SINTAX,
                       "missing identifier of function at " +
                           tkStream_.current()->locInfo());
  }
  const Token id{*tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  std::expected<std::vector<GenericParameter>, Error> generics{};
  if (tkStream_.current()->type() == TokenType::OPERATOR_SMALLER) {
    generics = parseGenerics();
  }
  if (!generics) {
    return createError(generics.error());
  }
  if (tkStream_.current()->type() != TokenType::LP) {
    return createError(ERROR_TYPE::SINTAX, "missing ( of function at " +
                                               tkStream_.current()->locInfo());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  const std::expected<Parameters, Error> params{parseParams()};
  if (!params) {
    return createError(params.error());
  }
  if (tkStream_.current()->type() != TokenType::RP) {
    return createError(ERROR_TYPE::SINTAX, "missing ) of function at " +
                                               tkStream_.current()->locInfo());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() != TokenType::DOTDOT) {
    return createError(ERROR_TYPE::SINTAX,
                       "missing : after ) of function decl at " +
                           tkStream_.current()->raw() + " at " +
                           tkStream_.current()->locInfo());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  const std::expected<std::shared_ptr<Type>, Error> returnType{parseType()};
  if (!returnType) {
    return createError(returnType.error());
  }
  const std::expected<std::shared_ptr<AST_BODY>, Error> body{parseBody()};
  if (!body || !*body) {
    return createError(body ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                            : body.error());
  }
  return Builder::createFuncDecl(
      SourceLocation{*firsToken, *tkStream_.lastRead()}, id.raw(), *generics,
      *params, *returnType, *body);
}

auto TopDown::parseParams() const noexcept -> std::expected<Parameters, Error> {
  std::vector<std::pair<std::string, std::shared_ptr<Type>>> params{};
  while (tkStream_.currentPos() < tkStream_.size() and
         tkStream_.current()->type() != TokenType::RP) {
    if (tkStream_.current()->type() != TokenType::ID) {
      return createError(ERROR_TYPE::SINTAX,
                         "missing id of param of function decl at " +
                             tkStream_.current()->raw() + " at " +
                             tkStream_.current()->locInfo());
    }
    const Token id{*tkStream_.current()};
    if (std::expected<std::monostate, Error> res = tryEat(); !res) {
      return createError(res.error());
    }
    if (tkStream_.current()->type() != TokenType::DOTDOT) {
      return createError(ERROR_TYPE::SINTAX,
                         "missing : after param of function decl at " +
                             tkStream_.current()->raw() + " at " +
                             tkStream_.current()->locInfo());
    }
    if (std::expected<std::monostate, Error> res = tryEat(); !res) {
      return createError(res.error());
    }
    const std::expected<std::shared_ptr<Type>, Error> returnType{parseType()};
    if (!returnType) {
      return createError(returnType.error());
    }
    params.push_back({id.raw(), *returnType});
    if (tkStream_.current()->type() == TokenType::COMMA) {
      if (std::expected<std::monostate, Error> res = tryEat(); !res) {
        return createError(res.error());
      }
      continue;
    } else if (tkStream_.current()->type() != TokenType::RP) {
      return createError(ERROR_TYPE::SINTAX,
                         "missing comma or parenthesis of function decl at " +
                             tkStream_.current()->locInfo());
    }
    break;
  }
  return Parameters{params};
}

auto TopDown::parseReturn() const noexcept
    -> std::expected<std::shared_ptr<AST_RETURN>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() == TokenType::SEMICOLON) {
    return Builder::createReturn(
        SourceLocation{*firsToken, *tkStream_.lastRead()}, nullptr);
  }
  const std::expected<std::shared_ptr<AST>, Error> value{parseTernary()};
  if (!value || !*value) {
    return createError(value ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                             : value.error());
  }
  if (tkStream_.current()->type() != TokenType::SEMICOLON) {
    return createError(ERROR_TYPE::SINTAX, "missing ; of return at " +
                                               tkStream_.current()->locInfo());
  }
  return Builder::createReturn(
      SourceLocation{*firsToken, *tkStream_.lastRead()}, *value);
}

auto TopDown::parseReplacementOfGenerics() const noexcept
    -> std::expected<std::vector<std::shared_ptr<Type>>, Error> {
  std::vector<std::shared_ptr<Type>> replacemments{};
  if (tkStream_.current()->type() == TokenType::OPERATOR_SMALLER and
      tkStream_.hasMatchingPairBefore(TokenType::OPERATOR_SMALLER,
                                      TokenType::OPERATOR_GREATER)) {
    std::expected<std::monostate, Error> res =
        tryEat(); // consume el identificador
    if (!res) {
      return createError(res.error());
    }
    std::expected<std::shared_ptr<Type>, Error> argExpected = parseType();
    if (!argExpected) {
      return createError(argExpected.error());
    }
    replacemments.push_back(argExpected.value());
    // Se admiten múltiples argumentos separados por ','
    while (tkStream_.current() &&
           tkStream_.current()->type() == TokenType::COMMA) {
      res = tryEat(); // consume ','
      if (!res) {
        return createError(res.error());
      }
      argExpected = parseType();
      if (!argExpected) {
        return createError(argExpected.error());
      }
      replacemments.push_back(argExpected.value());
    }
    if (!tkStream_.current() ||
        tkStream_.current()->type() != TokenType::OPERATOR_GREATER) {
      return createError(
          ERROR_TYPE::SINTAX,
          "Se esperaba '>' para cerrar los argumentos genéricos en " +
              tkStream_.lastRead()->locInfo());
    }
    res = tryEat(); // consume '>'
    if (!res) {
      return createError(res.error());
    }
  }
  return replacemments;
}

} // namespace nicole