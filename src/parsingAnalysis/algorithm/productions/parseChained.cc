#include "../../../../inc/parsingAnalysis/algorithm/topDown.h"
#include <expected>
#include <memory>
#include <vector>

namespace nicole {

auto TopDown::parseChainedExpression() const noexcept
    -> std::expected<std::shared_ptr<AST_CHAINED>, Error> {
  const std::expected<Token, Error> firsToken{tkStream_.current()};
  if (tkStream_.current()->type() != TokenType::ID) {
    return createError(ERROR_TYPE::SINTAX, "Expected identifier at " +
                                               tkStream_.current()->locInfo());
  }

  const Token baseToken{*tkStream_.current()};
  if (std::expected<std::monostate, Error> res = tryEat(); !res) {
    return createError(res.error());
  }

  std::expected<std::vector<std::shared_ptr<Type>>, Error>
      replacementOfGenerics{parseReplacementOfGenerics()};
  bool cannotBeVarCall{false};
  if (!replacementOfGenerics) {
    return createError(replacementOfGenerics.error());
  }

  if ((tkStream_.current()->type() != TokenType::LP or
       tkStream_.current()->type() != TokenType::LB) and
      replacementOfGenerics->size()) {
    cannotBeVarCall = true;
  }

  // 2. Determinar si es un simple varCall o una funcCall
  std::shared_ptr<AST> basePtr = nullptr;
  if (tkStream_.current()->type() == TokenType::LP) {
    const std::expected<std::vector<std::shared_ptr<AST>>, Error> arguemnts{
        parseArguments({TokenType::LP, TokenType::RP}, true)};
    if (!arguemnts) {
      return createError(arguemnts.error());
    }

    // Crear nodo de función
    std::expected<std::shared_ptr<AST_FUNC_CALL>, Error> funcCall =
        Builder::createFunCall(
            SourceLocation{*firsToken, *tkStream_.lastRead()}, baseToken.raw(),
            *replacementOfGenerics, *arguemnts);
    if (!funcCall || !*funcCall) {
      return createError(
          funcCall ? Error{ERROR_TYPE::NULL_NODE, "Failed to create func call"}
                   : funcCall.error());
    }
    basePtr = *funcCall;
  } else if (tkStream_.current()->type() == TokenType::LB) {

    const std::expected<std::vector<std::shared_ptr<AST>>, Error> arguemnts{
        parseArguments({TokenType::LB, TokenType::RB}, true)};
    if (!arguemnts) {
      return createError(arguemnts.error());
    }

    std::expected<std::shared_ptr<AST_CONSTRUCTOR_CALL>, Error>
        constructorCall = Builder::createConstructorCall(
            SourceLocation{*firsToken, *tkStream_.lastRead()}, baseToken.raw(),
            *replacementOfGenerics, *arguemnts);
    if (!constructorCall || !*constructorCall) {
      return createError(constructorCall
                             ? Error{ERROR_TYPE::NULL_NODE,
                                     "Failed to create constructor call"}
                             : constructorCall.error());
    }
    basePtr = *constructorCall;
  } else {
    if (cannotBeVarCall) {
      return createError(
          ERROR_TYPE::SINTAX,
          "a variable call cannot have replacements of generics, at" +
              tkStream_.lastRead()->locInfo());
    }
    // Variable normal
    std::expected<std::shared_ptr<AST_VAR_CALL>, Error> varCall =
        Builder::createVarCall(
            SourceLocation{*firsToken, *tkStream_.lastRead()}, baseToken.raw());
    if (!varCall || !*varCall) {
      return createError(
          varCall ? Error{ERROR_TYPE::NULL_NODE, "Failed to create var call"}
                  : varCall.error());
    }
    basePtr = *varCall;
  }

  // 3. Operaciones encadenadas: [index], .atributo, .metodo(...)
  std::vector<std::shared_ptr<AST>> operations;
  while (tkStream_.currentPos() < tkStream_.size() and
         (tkStream_.current()->type() == TokenType::LC or
          tkStream_.current()->type() == TokenType::DOT)) {
    TokenType ttype = tkStream_.current()->type();
    switch (ttype) {
    // Acceso a índice -> base[index]
    case TokenType::LC: {
      // Consumir '['
      if (std::expected<std::monostate, Error> res = tryEat(); !res) {
        return createError(res.error());
      }
      // parseOr() para la expresión de índice
      std::expected<std::shared_ptr<AST>, Error> indexExpr = parseOr();
      if (!indexExpr || !*indexExpr) {
        return createError(indexExpr
                               ? Error{ERROR_TYPE::NULL_NODE, "index is null"}
                               : indexExpr.error());
      }
      // Consumir ']'
      if (tkStream_.current()->type() != TokenType::RC || !tkStream_.eat()) {
        return createError(ERROR_TYPE::SINTAX,
                           "Missing ']' at " + tkStream_.current()->locInfo());
      }
      // Crear nodo de índice
      std::expected<std::shared_ptr<AST_INDEX>, Error> indexNode =
          Builder::createIndex(
              SourceLocation{*firsToken, *tkStream_.lastRead()}, *indexExpr);
      if (!indexNode || !*indexNode) {
        return createError(indexNode ? Error{ERROR_TYPE::NULL_NODE,
                                             "Failed to create index node"}
                                     : indexNode.error());
      }
      operations.push_back(*indexNode);
      break;
    }

    // Punto -> base.atributo o base.metodo(...)
    case TokenType::DOT: {
      // Consumir '.'
      if (std::expected<std::monostate, Error> res = tryEat(); !res) {
        return createError(res.error());
      }
      if (tkStream_.current()->type() != TokenType::ID) {
        return createError(ERROR_TYPE::SINTAX,
                           "Expected identifier after '.' at " +
                               tkStream_.current()->locInfo());
      }

      // Leer el nombre del atributo/método
      Token attrToken{*tkStream_.current()};
      if (std::expected<std::monostate, Error> res = tryEat(); !res) {
        return createError(res.error());
      }

      std::expected<std::vector<std::shared_ptr<Type>>, Error>
          replacementOfGenerics2{parseReplacementOfGenerics()};
      bool cannotBeAttrAccess{false};
      if (!replacementOfGenerics2) {
        return createError(replacementOfGenerics2.error());
      }

      if (tkStream_.current()->type() != TokenType::LP and
          replacementOfGenerics->size()) {
        cannotBeAttrAccess = true;
      }

      // Comprobar si es método -> un '(' a continuación
      if (tkStream_.current()->type() == TokenType::LP) {

        const std::expected<std::vector<std::shared_ptr<AST>>, Error> arguemnts{
            parseArguments({TokenType::LP, TokenType::RP}, true)};
        if (!arguemnts) {
          return createError(arguemnts.error());
        }

        std::expected<std::shared_ptr<AST_METHOD_CALL>, Error> methodNode =
            Builder::createMethodCall(
                SourceLocation{*firsToken, *tkStream_.lastRead()},
                attrToken.raw(), *replacementOfGenerics2, *arguemnts);
        if (!methodNode || !*methodNode) {
          return createError(methodNode ? Error{ERROR_TYPE::NULL_NODE,
                                                "Failed to create method call"}
                                        : methodNode.error());
        }
        operations.push_back(*methodNode);
      } else {
        if (cannotBeAttrAccess) {
          return createError(
              ERROR_TYPE::SINTAX,
              "a attr access cannot have replacements of generics, at" +
                  tkStream_.lastRead()->locInfo());
        }
        // Es un atributo
        std::expected<std::shared_ptr<AST_ATTR_ACCESS>, Error> attrNode =
            Builder::createAttrAccess(
                SourceLocation{*firsToken, *tkStream_.lastRead()},
                attrToken.raw());
        if (!attrNode || !*attrNode) {
          return createError(attrNode
                                 ? Error{ERROR_TYPE::NULL_NODE,
                                         "Failed to create attribute access"}
                                 : attrNode.error());
        }
        operations.push_back(*attrNode);
      }
      break;
    }

    default:
      // Si ya no es ni '[', ni '.', ni '(', terminamos
      return Builder::createChained(
          SourceLocation{*firsToken, *tkStream_.lastRead()}, basePtr,
          operations);
    }
  }

  // Si se acaban los tokens, retornamos la cadena
  return Builder::createChained(
      SourceLocation{*firsToken, *tkStream_.lastRead()}, basePtr, operations);
}

} // namespace nicole