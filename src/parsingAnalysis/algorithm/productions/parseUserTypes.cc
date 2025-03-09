#include "../../../../inc/parsingAnalysis/algorithm/topDown.h"
#include <memory>

namespace nicole {

const std::expected<std::shared_ptr<AST_ENUM>, Error>
TopDown::parseEnum() const noexcept {
  if (auto res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() != TokenType::ID) {
    return createError(ERROR_TYPE::SINTAX, "missing enum identifier at " +
                                               tkStream_.current()->locInfo());
  }
  const Token id{*tkStream_.current()};
  if (auto res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() != TokenType::LB) {
    return createError(ERROR_TYPE::SINTAX, "missing { of enum at " +
                                               tkStream_.current()->locInfo());
  }
  if (auto res = tryEat(); !res) {
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
    if (auto res = tryEat(); !res) {
      return createError(res.error());
    }
    if (tkStream_.current()->type() == TokenType::COMMA) {
      if (auto res = tryEat(); !res) {
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
  if (auto res = tryEat(); !res) {
    return createError(res.error());
  }
  return Builder::createEnum(id.raw(), identifiers);
}

const std::expected<std::shared_ptr<AST_ENUM_ACCESS>, Error>
TopDown::parseEnumAccess() const noexcept {
  const Token id{*tkStream_.current()};
  if (auto res = tryEat(); !res) {
    return createError(res.error());
  }
  if (auto res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() != TokenType::DOTDOT) {
    return createError(ERROR_TYPE::SINTAX, "missing : in enum access at " +
                                               tkStream_.lastRead()->locInfo());
  }
  if (auto res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() != TokenType::ID) {
    return createError(ERROR_TYPE::SINTAX,
                       "missing identifier in enum access at " +
                           tkStream_.lastRead()->locInfo());
  }
  const Token identifier{*tkStream_.current()};
  if (auto res = tryEat(); !res) {
    return createError(res.error());
  }
  return Builder::createEnumAccess(id.raw(), identifier.raw());
}

const std::expected<std::shared_ptr<AST_STRUCT>, Error>
TopDown::parseStructDecl() const noexcept {
  if (auto res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() != TokenType::ID) {
    return createError(ERROR_TYPE::SINTAX, "missing struct identifier at " +
                                               tkStream_.current()->locInfo());
  }
  const Token id{*tkStream_.current()};
  if (auto res = tryEat(); !res) {
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
    if (auto res = tryEat(); !res) {
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
  if (auto res = tryEat(); !res) {
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
      if (auto res = tryEat(); !res) {
        return createError(res.error());
      }
      if (tkStream_.current()->type() != TokenType::DOTDOT) {
        return createError(ERROR_TYPE::SINTAX,
                           "missing : of attr at " +
                               tkStream_.current()->locInfo());
      }
      if (auto res = tryEat(); !res) {
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
      constructor = (fatherType) ? parseConstructorDecl(id.raw(), *fatherType)
                                 : parseConstructorDecl(id.raw(), nullptr);
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
      destructor = parseDestructorDecl(id.raw());
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

  if (auto res = tryEat(); !res) {
    return createError(res.error());
  }
  return Builder::createStruct(id.raw(), *generics, *fatherType,
                               Attributes{params}, methods, *constructor,
                               *destructor);
}

const std::expected<std::shared_ptr<AST_CONSTRUCTOR_DECL>, Error>
TopDown::parseConstructorDecl(
    const std::string &id_returnType,
    const std::shared_ptr<Type> &fatherType) const noexcept {
  if (auto res = tryEat(); !res) {
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
  if (auto res = tryEat(); !res) {
    return createError(res.error());
  }
  const auto params{parseParams()};
  if (!params) {
    return createError(params.error());
  }
  if (tkStream_.current()->type() != TokenType::RP) {
    return createError(ERROR_TYPE::SINTAX, "missing ) of function at " +
                                               tkStream_.current()->locInfo());
  }
  if (auto res = tryEat(); !res) {
    return createError(res.error());
  }
  std::expected<std::shared_ptr<AST_SUPER>, Error> super{nullptr};
  if (fatherType) {
    if (tkStream_.current()->type() != TokenType::DOTDOT) {
      return createError(ERROR_TYPE::SINTAX,
                         "missing : at " + tkStream_.current()->locInfo());
    }
    if (auto res = tryEat(); !res) {
      return createError(res.error());
    }
    if (tkStream_.current()->type() != TokenType::SUPER) {
      return createError(ERROR_TYPE::SINTAX,
                         "missing super at " + tkStream_.current()->locInfo());
    }
    if (auto res = tryEat(); !res) {
      return createError(res.error());
    }
    const auto replacements{parseReplacementOfGenerics()};
    if (!replacements) {
      return createError(replacements.error());
    }
    const auto arguments{parseArguments({TokenType::LP, TokenType::RP}, true)};
    if (!arguments) {
      return createError(arguments.error());
    }
    super = Builder::createSuper(fatherType, *replacements, *arguments);
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
      id_returnType, *generics, *params, *super,
      std::make_shared<UserType>(id_returnType, nullptr,
                                 std::vector<GenericParameter>{}),
      *body);
}

const std::expected<std::shared_ptr<AST_DESTRUCTOR_DECL>, Error>
TopDown::parseDestructorDecl(const std::string &id) const noexcept {
  if (auto res = tryEat(); !res) {
    return createError(res.error());
  }
  const auto body{parseBody()};
  if (!body || !*body) {
    return createError(body ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                            : body.error());
  }
  return Builder::createDestructorDecl(id, *body);
}

const std::expected<std::shared_ptr<AST_METHOD_DECL>, Error>
TopDown::parseMethodDecl(const bool isVirtual) const noexcept {
  if (auto res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() != TokenType::ID) {
    return createError(ERROR_TYPE::SINTAX,
                       "missing identifier of function at " +
                           tkStream_.current()->locInfo());
  }
  const Token id{*tkStream_.current()};
  if (auto res = tryEat(); !res) {
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
  if (auto res = tryEat(); !res) {
    return createError(res.error());
  }
  const auto params{parseParams()};
  if (!params) {
    return createError(params.error());
  }
  if (tkStream_.current()->type() != TokenType::RP) {
    return createError(ERROR_TYPE::SINTAX, "missing ) of function at " +
                                               tkStream_.current()->locInfo());
  }
  if (auto res = tryEat(); !res) {
    return createError(res.error());
  }
  if (tkStream_.current()->type() != TokenType::DOTDOT) {
    return createError(ERROR_TYPE::SINTAX,
                       "missing : after ) of function decl at " +
                           tkStream_.current()->raw() + " at " +
                           tkStream_.current()->locInfo());
  }
  if (auto res = tryEat(); !res) {
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
  return Builder::createMethodDecl(id.raw(), *generics, *params, *returnType,
                                   *body, isVirtual);
}

} // namespace nicole