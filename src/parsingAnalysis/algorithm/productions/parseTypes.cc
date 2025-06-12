#include "../../../../inc/parsingAnalysis/algorithm/topDown.h"
#include <memory>
#include <vector>

namespace nicole {

auto TopDown::parseType() const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  // Se parsea el tipo base (incluyendo genéricos o vectores)
  std::expected<std::shared_ptr<Type>, Error> baseTypeExpected =
      parsePrimaryType();
  if (!baseTypeExpected) {
    return createError(baseTypeExpected.error());
  }
  std::shared_ptr<Type> type = baseTypeExpected.value();

  // Se admiten múltiples operadores '*' para punteros
  while (tkStream_.current() &&
         tkStream_.current()->type() == TokenType::OPERATOR_MULT) {
    std::expected<std::monostate, Error> res = tryEat(); // consume '*'
    if (!res) {
      return createError(res.error());
    }
    type = std::make_shared<PointerType>(type);
  }
  return type;
}

auto TopDown::parsePrimaryType() const noexcept
    -> std::expected<std::shared_ptr<Type>, Error> {
  const std::expected<Token, Error> tokenExpected = tkStream_.current();
  if (!tokenExpected) {
    return createError(ERROR_TYPE::SINTAX,
                       "Fin de entrada inesperado al parsear un tipo.");
  }
  const Token &token = *tokenExpected;
  switch (token.type()) {
  case TokenType::VECTOR: {
    // Parsea "vector<elementType>"
    std::expected<std::monostate, Error> res = tryEat(); // consume 'vector'
    if (!res) {
      return createError(res.error());
    }
    if (!tkStream_.current() ||
        tkStream_.current()->type() != TokenType::OPERATOR_SMALLER) {
      return createError(ERROR_TYPE::SINTAX,
                         "Se esperaba '<' después de 'vector' en " +
                             token.locInfo());
    }
    res = tryEat(); // consume '<'
    if (!res) {
      return createError(res.error());
    }
    std::expected<std::shared_ptr<Type>, Error> elemTypeExpected = parseType();
    if (!elemTypeExpected) {
      return createError(elemTypeExpected.error());
    }
    if (!tkStream_.current() ||
        tkStream_.current()->type() != TokenType::OPERATOR_GREATER) {
      return createError(ERROR_TYPE::SINTAX,
                         "Se esperaba '>' tras el tipo de elemento en " +
                             token.locInfo());
    }
    res = tryEat(); // consume '>'
    if (!res) {
      return createError(res.error());
    }
    return std::make_shared<VectorType>(elemTypeExpected.value());
  }
  case TokenType::ID: {
    // Parsea un tipo definido por el usuario, con posibilidad de argumentos
    // genéricos.
    std::string name =
        token.raw(); // se asume que token.value() retorna el nombre
    std::expected<std::monostate, Error> res =
        tryEat(); // consume el identificador
    if (!res) {
      return createError(res.error());
    } // Si sigue un '<', se parsean los argumentos genéricos
    if (tkStream_.current() &&
        tkStream_.current()->type() == TokenType::OPERATOR_SMALLER) {
      res = tryEat(); // consume '<'
      if (!res) {
        return createError(res.error());
      }
      std::vector<std::shared_ptr<Type>> typeArgs;
      std::expected<std::shared_ptr<Type>, Error> argExpected = parseType();
      if (!argExpected) {
        return createError(argExpected.error());
      }
      typeArgs.push_back(argExpected.value());
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
        typeArgs.push_back(argExpected.value());
      }
      if (!tkStream_.current() ||
          tkStream_.current()->type() != TokenType::OPERATOR_GREATER) {
        return createError(
            ERROR_TYPE::SINTAX,
            "Se esperaba '>' para cerrar los argumentos genéricos en " +
                token.locInfo());
      }
      res = tryEat(); // consume '>'
      if (!res) {
        return createError(res.error());
      } // Se crea un GenericInstanceType a partir de un UserType provisional
      std::shared_ptr<UserType> userType = std::make_shared<UserType>(
          name, nullptr, std::vector<GenericParameter>{});
      return std::make_shared<GenericInstanceType>(userType, typeArgs);
    } else {
      // Tipo de usuario simple sin argumentos genéricos
      return std::make_shared<UserType>(name, nullptr,
                                        std::vector<GenericParameter>{});
    }
  }
  case TokenType::TYPE_VOID:
  case TokenType::TYPE_INT:
  case TokenType::TYPE_BOOL:
  case TokenType::TYPE_CHAR:
  case TokenType::TYPE_STR:
  case TokenType::TYPE_FLOAT:
  case TokenType::TYPE_DOUBLE: {
    std::expected<std::monostate, Error> res =
        tryEat(); // consume el tipo incorporado
    if (!res) {
      return createError(res.error());
    }
    switch (token.type()) {
    case TokenType::TYPE_VOID:
      return std::make_shared<VoidType>();
    case TokenType::TYPE_INT:
      return std::make_shared<BasicType>(BasicKind::Int);
    case TokenType::TYPE_BOOL:
      return std::make_shared<BasicType>(BasicKind::Bool);
    case TokenType::TYPE_CHAR:
      return std::make_shared<BasicType>(BasicKind::Char);
    case TokenType::TYPE_STR:
      return std::make_shared<BasicType>(BasicKind::Str);
    case TokenType::TYPE_FLOAT:
      return std::make_shared<BasicType>(BasicKind::Float);
    case TokenType::TYPE_DOUBLE:
      return std::make_shared<BasicType>(BasicKind::Double);
    default:
      break;
    }
  }
  default:
    return createError(ERROR_TYPE::SINTAX,
                       "Falta un tipo en " + token.locInfo());
  }
  return createError(ERROR_TYPE::SINTAX,
                     "Error inesperado en parsePrimaryType.");
}

auto TopDown::parseGenerics() const noexcept
    -> std::expected<std::vector<GenericParameter>, Error> {
  std::vector<GenericParameter> result{};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  while (tkStream_.currentPos() < tkStream_.size() and
         tkStream_.current()->type() != TokenType::OPERATOR_GREATER) {
    if (tkStream_.current()->type() != TokenType::ID) {
      return createError(ERROR_TYPE::SINTAX,
                         "misplaced token " + tkStream_.current()->raw() +
                             " at " + tkStream_.current()->locInfo());
    }
    result.push_back(GenericParameter{tkStream_.current()->raw()});
    if (std::expected<std::monostate, Error> res = tryEat(); !res) {
      return createError(res.error());
    }
    if (tkStream_.current()->type() == TokenType::COMMA) {
      if (std::expected<std::monostate, Error> res = tryEat(); !res) {
        return createError(res.error());
      }
      continue;
    }
  }
  if (!result.size()) {
    return createError(ERROR_TYPE::SINTAX,
                       "no generic type has been specified at" +
                           tkStream_.current()->locInfo());
  }
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  return result;
}

} // namespace nicole

// const name: type = value;
// let name: type = value;
// def fun<Generics>(id: type): type {}
// struct name: extends type<Generics> {}
// constructorDecl<Generics>(id: type) {}
// constructorCall<Generics>(){}
// funCall<Generics>()()