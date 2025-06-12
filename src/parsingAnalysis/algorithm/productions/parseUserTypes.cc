#include "../../../../inc/parsingAnalysis/algorithm/topDown.h"
#include <memory>

namespace nicole {

auto TopDown::parseEnum() const noexcept
    -> std::expected<std::shared_ptr<AST_ENUM>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() != TokenType::ID) {
    return createError(ERROR_TYPE::SINTAX, "missing enum identifier at " +
                                               tkStream_.current()->locInfo());
  }
  const Token id{*tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() != TokenType::LB) {
    return createError(ERROR_TYPE::SINTAX, "missing { of enum at " +
                                               tkStream_.current()->locInfo());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  std::vector<std::string> identifiers{};
  while (tkStream_.currentPos() < tkStream_.size() and
         tkStream_.current()->type() != TokenType::RB) {

    if (tkStream_.current()->type() != TokenType::ID) {
      return createError(ERROR_TYPE::SINTAX,
                         "failed to eat " + tkStream_.current()->raw() +
                             " at " + tkStream_.current()->locInfo());
    }
    identifiers.push_back(tkStream_.current()->raw());
    if (std::expected<std::monostate, Error> res = tryEat(); !res) {
      return createError(res.error());
    }
    if (tkStream_.current()->type() == TokenType::COMMA) {
      if (std::expected<std::monostate, Error> res = tryEat(); !res) {
        return createError(res.error());
      }
      continue;
    } else if (tkStream_.current()->type() != TokenType::RB) {
      return createError(ERROR_TYPE::SINTAX,
                         "missing comma or parenthesis of enum at " +
                             tkStream_.current()->locInfo());
    }
    break;
  }
  if (tkStream_.current()->type() != TokenType::RB) {
    return createError(ERROR_TYPE::SINTAX, "missing } of enum at " +
                                               tkStream_.current()->locInfo());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  return Builder::createEnum(SourceLocation{*firsToken, *tkStream_.lastRead()},
                             id.raw(), identifiers);
}

auto TopDown::parseEnumAccess() const noexcept
    -> std::expected<std::shared_ptr<AST_ENUM_ACCESS>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  const Token id{*tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() != TokenType::DOTDOT) {
    return createError(ERROR_TYPE::SINTAX, "missing : in enum access at " +
                                               tkStream_.lastRead()->locInfo());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() != TokenType::ID) {
    return createError(ERROR_TYPE::SINTAX,
                       "missing identifier in enum access at " +
                           tkStream_.lastRead()->locInfo());
  }
  const Token identifier{*tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  return Builder::createEnumAccess(
      SourceLocation{*firsToken, *tkStream_.lastRead()}, id.raw(),
      identifier.raw());
}

auto TopDown::parseStructDecl() const noexcept
    -> std::expected<std::shared_ptr<AST_STRUCT>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() != TokenType::ID) {
    return createError(ERROR_TYPE::SINTAX, "missing struct identifier at " +
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
  std::expected<std::shared_ptr<Type>, Error> fatherType{nullptr};
  if (tkStream_.current()->type() == TokenType::EXTENDS) {
    if (std::expected<std::monostate, Error> res = tryEat(); !res) {
      return createError(res.error());
    }
    fatherType = parseType();
    if (!fatherType) {
      return createError(fatherType.error());
    }
  }

  if (tkStream_.current()->type() != TokenType::LB) {
    return createError(ERROR_TYPE::SINTAX, "missing { of struct at " +
                                               tkStream_.current()->locInfo());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }

  bool isConstructorParsed{false};
  bool isDestructorParsed{false};
  std::vector<std::shared_ptr<AST_METHOD_DECL>> methods{};
  std::vector<std::pair<std::string, std::shared_ptr<Type>>> params{};
  std::expected<std::shared_ptr<AST_CONSTRUCTOR_DECL>, Error> constructor{
      nullptr};
  std::expected<std::shared_ptr<AST_DESTRUCTOR_DECL>, Error> destructor{
      nullptr};

  while (tkStream_.currentPos() < tkStream_.size() and
         tkStream_.current()->type() != TokenType::RB) {
    switch (tkStream_.current()->type()) {

    case TokenType::ID: {
      const Token attrID{*tkStream_.current()};
      if (std::expected<std::monostate, Error> res = tryEat(); !res) {
        return createError(res.error());
      }
      if (tkStream_.current()->type() != TokenType::DOTDOT) {
        return createError(ERROR_TYPE::SINTAX,
                           "missing : of attr at " +
                               tkStream_.current()->locInfo());
      }
      if (std::expected<std::monostate, Error> res = tryEat(); !res) {
        return createError(res.error());
      }
      const std::expected<std::shared_ptr<Type>, Error> type{parseType()};
      if (!type) {
        return createError(type.error());
      }
      params.push_back({attrID.raw(), *type});
      break;
    }

    case TokenType::METHOD: {
      const std::expected<std::shared_ptr<AST_METHOD_DECL>, Error> method{
          parseMethodDecl(false)};
      if (!method || !*method) {
        return createError(method ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                                  : method.error());
      }
      methods.push_back(*method);
      break;
    }

    case TokenType::VIRTUAL: {
      if (tkStream_.lookAhead(1)->type() == TokenType::DESTRUCTOR) {
        if (isDestructorParsed) {
          return createError(ERROR_TYPE::SINTAX,
                             "destructor overloading not implemented, at " +
                                 tkStream_.current()->locInfo());
        }
        isDestructorParsed = true;
        destructor = parseDestructorDecl(id.raw(), true);
        if (!destructor || !*destructor) {
          return createError(destructor
                                 ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                                 : destructor.error());
        }
        break;
      }
      const std::expected<std::shared_ptr<AST_METHOD_DECL>, Error> method{
          parseMethodDecl(true)};
      if (!method || !*method) {
        return createError(method ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                                  : method.error());
      }
      methods.push_back(*method);
      break;
    }

    case TokenType::CONSTRUCTOR: {
      if (isConstructorParsed) {
        return createError(ERROR_TYPE::SINTAX,
                           "constructor overloading not implemented, at " +
                               tkStream_.current()->locInfo());
      }
      isConstructorParsed = true;
      constructor = (fatherType)
                        ? parseConstructorDecl(id.raw(), *fatherType, *generics)
                        : parseConstructorDecl(id.raw(), nullptr, *generics);
      if (!constructor || !*constructor) {
        return createError(constructor
                               ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                               : constructor.error());
      }
      break;
    }

    case TokenType::DESTRUCTOR: {
      if (isDestructorParsed) {
        return createError(ERROR_TYPE::SINTAX,
                           "destructor overloading not implemented, at " +
                               tkStream_.current()->locInfo());
      }
      isDestructorParsed = true;
      destructor = parseDestructorDecl(id.raw(), false);
      if (!destructor || !*destructor) {
        return createError(destructor
                               ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                               : destructor.error());
      }
      break;
    }

    default: {
      if (tkStream_.current()->type() != TokenType::RB) {
        return createError(ERROR_TYPE::SINTAX,
                           "token in wrong position at " +
                               tkStream_.current()->locInfo());
      }
    }
    }
  }

  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  return Builder::createStruct(
      SourceLocation{*firsToken, *tkStream_.lastRead()}, id.raw(), *generics,
      *fatherType, Attributes{params}, methods, *constructor, *destructor);
}

auto TopDown::parseConstructorDecl(
    const std::string &id_returnType, const std::shared_ptr<Type> &fatherType,
    const std::vector<GenericParameter> &classGenerics) const noexcept
    -> std::expected<std::shared_ptr<AST_CONSTRUCTOR_DECL>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  /*
  std::expected<std::vector<GenericParameter>, Error> generics{};
  if (tkStream_.current()->type() == TokenType::OPERATOR_SMALLER) {
    generics = parseGenerics();
  }
  if (!generics) {
    return createError(generics.error());
  }
  */
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
  std::expected<std::shared_ptr<AST_SUPER>, Error> super{nullptr};
  if (fatherType) {
    if (tkStream_.current()->type() != TokenType::DOTDOT) {
      return createError(ERROR_TYPE::SINTAX,
                         "missing : at " + tkStream_.current()->locInfo());
    }
    if (std::expected<std::monostate, Error> res = tryEat(); !res) {
      return createError(res.error());
    }
    if (tkStream_.current()->type() != TokenType::SUPER) {
      return createError(ERROR_TYPE::SINTAX,
                         "missing super at " + tkStream_.current()->locInfo());
    }
    if (std::expected<std::monostate, Error> res = tryEat(); !res) {
      return createError(res.error());
    }
    const std::expected<std::vector<std::shared_ptr<Type>>, Error> replacements{
        parseReplacementOfGenerics()};
    if (!replacements) {
      return createError(replacements.error());
    }
    const std::expected<std::vector<std::shared_ptr<AST>>, Error> arguments{
        parseArguments({TokenType::LP, TokenType::RP}, true)};
    if (!arguments) {
      return createError(arguments.error());
    }
    super =
        Builder::createSuper(SourceLocation{*firsToken, *tkStream_.lastRead()},
                             fatherType, *replacements, *arguments);
    if (!super) {
      return createError(super.error());
    }
  }
  const std::expected<std::shared_ptr<AST_BODY>, Error> body{parseBody()};
  if (!body || !*body) {
    return createError(body ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                            : body.error());
  }
  return Builder::createConstructorDecl(
      SourceLocation{*firsToken, *tkStream_.lastRead()}, id_returnType,
      classGenerics, *params, *super,
      std::make_shared<UserType>(id_returnType, nullptr,
                                 std::vector<GenericParameter>{}),
      *body);
}

auto TopDown::parseDestructorDecl(const std::string &id,
                                  const bool isVirtual) const noexcept
    -> std::expected<std::shared_ptr<AST_DESTRUCTOR_DECL>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  if (isVirtual) {
    if (std::expected<std::monostate, Error> res = tryEat(); !res) {
      return createError(res.error());
    }
  }
  const std::expected<std::shared_ptr<AST_BODY>, Error> body{parseBody()};
  if (!body || !*body) {
    return createError(body ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                            : body.error());
  }
  return Builder::createDestructorDecl(
      SourceLocation{*firsToken, *tkStream_.lastRead()}, id,
      std::make_shared<UserType>(id, nullptr, std::vector<GenericParameter>{}),
      *body, isVirtual);
}

auto TopDown::parseMethodDecl(const bool isVirtual) const noexcept
    -> std::expected<std::shared_ptr<AST_METHOD_DECL>, Error> {
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
  if (isVirtual and
      tkStream_.current()->type() == TokenType::OPERATOR_SMALLER) {
    return createError(ERROR_TYPE::SINTAX,
                       "a virtual method cannot be templated " +
                           tkStream_.current()->locInfo());
  }
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
  return Builder::createMethodDecl(
      SourceLocation{*firsToken, *tkStream_.lastRead()}, id.raw(), *generics,
      *params, *returnType, *body, isVirtual);
}

} // namespace nicole