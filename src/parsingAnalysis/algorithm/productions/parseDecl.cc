#include "../../../../inc/parsingAnalysis/algorithm/topDown.h"
#include <memory>

namespace nicole {

auto TopDown::parseVarDecl(const bool insideFor) const noexcept
    -> std::expected<std::shared_ptr<AST>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  const Token token{*tkStream_.current()};
  std::string varName;
  std::shared_ptr<Type> varType{nullptr};
  std::shared_ptr<AST> valueExpr;

  // Función auxiliar para manejar la parte común de las declaraciones
  auto parseCommon = [&]() -> std::expected<void, Error> {
    // Verificar si el siguiente token es un identificador (nombre de la
    // variable)
    if (tkStream_.current()->type() != TokenType::ID) {
      return createError(ERROR_TYPE::SINTAX,
                         "Expected variable name at " +
                             tkStream_.current()->locInfo());
    }
    varName = tkStream_.current()->raw();
    if (std::expected<std::monostate, Error> res = tryEat(); !res) {
      return createError(res.error());
    }

    // Verificar si hay una especificación de tipo
    if (tkStream_.current()->type() == TokenType::DOTDOT) {
      if (std::expected<std::monostate, Error> res = tryEat(); !res) {
        return createError(res.error());
      }
      const std::expected<std::shared_ptr<Type>, Error> returnType{parseType()};
      if (!returnType) {
        return createError(returnType.error());
      }
      varType = *returnType;
    }

    // Verificar si hay un operador de asignación '='
    if (tkStream_.current()->type() == TokenType::ASSIGNMENT) {
      if (std::expected<std::monostate, Error> res = tryEat(); !res) {
        return createError(res.error());
      }
      // Parsear la expresión de valor
      std::expected<std::shared_ptr<AST>, Error> expr = parseTernary();
      if (!expr || !*expr) {
        return createError(
            expr ? Error{ERROR_TYPE::NULL_NODE, "Expression is null"}
                 : expr.error());
      }
      valueExpr = *expr;
    } else {
      return createError(ERROR_TYPE::SINTAX,
                         "Expected '=' after variable declaration at " +
                             tkStream_.current()->locInfo());
    }

    // Verificar el punto y coma al final de la declaración
    if (!insideFor) {
      if (tkStream_.current()->type() != TokenType::SEMICOLON) {
        return createError(
            ERROR_TYPE::SINTAX,
            "Expected ';' at the end of variable declaration at " +
                tkStream_.current()->locInfo());
      }
      if (std::expected<std::monostate, Error> res = tryEat(); !res) {
        return createError(res.error());
      }
    }
    return {};
  };

  switch (token.type()) {
  case TokenType::CONST: { // const variable: type = expression;
    if (std::expected<std::monostate, Error> res = tryEat(); !res) {
      return createError(res.error());
    }
    std::expected<void, Error> result = parseCommon();
    if (!result) {
      return createError(result.error());
    }
    varType = std::make_shared<ConstType>(varType);
    return Builder::createVarTypedtDecl(
        SourceLocation{*firsToken, *tkStream_.lastRead()}, varName, varType,
        valueExpr);
  }
  case TokenType::LET: { // let variable: type = expression;
    if (std::expected<std::monostate, Error> res = tryEat(); !res) {
      return createError(res.error());
    }
    std::expected<void, Error> result = parseCommon();
    if (!result) {
      return createError(result.error());
    }
    return Builder::createVarTypedtDecl(
        SourceLocation{*firsToken, *tkStream_.lastRead()}, varName, varType,
        valueExpr);
  }
  case TokenType::AUTO: { // auto variable = expression;
    if (std::expected<std::monostate, Error> res = tryEat(); !res) {
      return createError(res.error());
    }
    std::expected<void, Error> result = parseCommon();
    if (!result) {
      return createError(result.error());
    }
    if (varType) {
      return createError(ERROR_TYPE::SINTAX,
                         "Auto variable should not have a type specified at " +
                             tkStream_.current()->locInfo());
    }
    return Builder::createAutoDecl(
        SourceLocation{*firsToken, *tkStream_.lastRead()}, varName, valueExpr,
        false);
  }
  default: {
    return parseTernary();
  }
  }
}

auto TopDown::parseDelete() const noexcept
    -> std::expected<std::shared_ptr<AST_DELETE>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }
  const std::expected<std::shared_ptr<AST>, Error> value{parseOr()};
  if (!value || !*value) {
    return createError(value ? Error{ERROR_TYPE::NULL_NODE, "node is null"}
                             : value.error());
  }
  if (tkStream_.current()->type() != TokenType::SEMICOLON) {
    return createError(ERROR_TYPE::SINTAX,
                       "Expected ';' at the end of delete expression at " +
                           tkStream_.current()->locInfo());
  }
  return Builder::createDelete(
      SourceLocation{*firsToken, *tkStream_.lastRead()}, *value);
}

} // namespace nicole
