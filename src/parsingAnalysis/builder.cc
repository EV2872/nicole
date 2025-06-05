#include "../../inc/parsingAnalysis/builder.h"
#include <memory>

namespace nicole {

std::expected<std::shared_ptr<AST_BOOL>, Error>
Builder::createBool(const SourceLocation &srcLoc, const bool value) noexcept {
  return std::make_shared<AST_BOOL>(generateNextId(), srcLoc, value);
}

std::expected<std::shared_ptr<AST_CHAR>, Error>
Builder::createChar(const SourceLocation &srcLoc,
                    const std::string &value) noexcept {
  const std::string strNoQuotes{value.substr(1, value.size() - 2)};
  char result;
  if (strNoQuotes.size() == 1) {
    result = strNoQuotes[0];
  }
  // Si hay un carácter de escape, comprobar cuál es
  if (strNoQuotes[0] == '\\' && strNoQuotes.size() == 2) {
    switch (strNoQuotes[1]) {
    case 'n':
      result = '\n';
    case 't':
      result = '\t';
    case '\\':
      result = '\\';
    case '\'':
      result = '\'';
    case '\"':
      result = '\"';
    default:
      result = strNoQuotes[1];
    }
  }
  return std::make_shared<AST_CHAR>(generateNextId(), srcLoc, result);
}

std::expected<std::shared_ptr<AST_DOUBLE>, Error>
Builder::createDouble(const SourceLocation &srcLoc,
                      const double value) noexcept {
  return std::make_shared<AST_DOUBLE>(generateNextId(), srcLoc, value);
}

std::expected<std::shared_ptr<AST_FLOAT>, Error>
Builder::createFloat(const SourceLocation &srcLoc, const float value) noexcept {
  return std::make_shared<AST_FLOAT>(generateNextId(), srcLoc, value);
}

std::expected<std::shared_ptr<AST_INT>, Error>
Builder::createInt(const SourceLocation &srcLoc, const int value) noexcept {
  return std::make_shared<AST_INT>(generateNextId(), srcLoc, value);
}

std::expected<std::shared_ptr<AST_NULL>, Error>
Builder::createNull(const SourceLocation &srcLoc) noexcept {
  return std::make_shared<AST_NULL>(generateNextId(), srcLoc);
}

std::expected<std::shared_ptr<AST_STRING>, Error>
Builder::createString(const SourceLocation &srcLoc,
                      const std::string value) noexcept {
  std::string result;
  if (value.size() < 2) {
    return createError(ERROR_TYPE::SINTAX, "ill formed string");
  }
  const std::string strNoQuotes = value.substr(1, value.size() - 2);
  for (size_t i = 0; i < strNoQuotes.length(); ++i) {
    if (strNoQuotes[i] == '\\' && i + 1 < strNoQuotes.length()) {
      switch (strNoQuotes[i + 1]) {
      case 'n':
        result.push_back('\n');
        break;
      case 't':
        result.push_back('\t');
        break;
      case '\\':
        result.push_back('\\');
        break;
      case '\"':
        result.push_back('\"');
        break;
      default:
        result.push_back(strNoQuotes[i + 1]);
        break;
      }
      i++; // Saltar el carácter escapado
    } else {
      result.push_back(strNoQuotes[i]);
    }
  }
  return std::make_shared<AST_STRING>(generateNextId(), srcLoc, result);
}

std::expected<std::shared_ptr<AST_VECTOR>, Error>
Builder::createVector(const SourceLocation &srcLoc,
                      const std::vector<std::shared_ptr<AST>> values) noexcept {
  const std::shared_ptr<AST_VECTOR> astVector{
      std::make_shared<AST_VECTOR>(generateNextId(), srcLoc, values)};
  const std::vector<std::shared_ptr<AST>> &expressions{astVector->values()};
  for (const std::shared_ptr<AST> &expression : expressions) {
    if (expression) {
      expression->setFather(astVector);
    }
  }
  return astVector;
}

std::expected<std::shared_ptr<AST_INDEX>, Error>
Builder::createIndex(const SourceLocation &srcLoc,
                     const std::shared_ptr<AST> value) noexcept {
  const std::shared_ptr<AST_INDEX> astIndex{
      std::make_shared<AST_INDEX>(generateNextId(), srcLoc, value)};
  if (value) {
    value->setFather(astIndex);
  }
  return astIndex;
}

std::expected<std::shared_ptr<AST_DELETE>, Error>
Builder::createDelete(const SourceLocation &srcLoc,
                      const std::shared_ptr<AST> &value) noexcept {
  const std::shared_ptr<AST_DELETE> astDelete{
      std::make_shared<AST_DELETE>(generateNextId(), srcLoc, value)};
  if (value) {
    value->setFather(astDelete);
  }
  return astDelete;
}

std::expected<std::shared_ptr<AST_NEW>, Error>
Builder::createNew(const SourceLocation &srcLoc,
                   const std::shared_ptr<AST> &value) noexcept {
  const std::shared_ptr<AST_NEW> astNew{
      std::make_shared<AST_NEW>(generateNextId(), srcLoc, value)};
  if (value) {
    value->setFather(astNew);
  }
  return astNew;
}

std::expected<std::shared_ptr<AST_DEREF>, Error>
Builder::createDeref(const SourceLocation &srcLoc,
                     const std::shared_ptr<AST> &value) noexcept {
  const std::shared_ptr<AST_DEREF> astDeref{
      std::make_shared<AST_DEREF>(generateNextId(), srcLoc, value)};
  if (value) {
    value->setFather(astDeref);
  }
  return astDeref;
}

std::expected<std::shared_ptr<AST_BINARY>, Error>
Builder::createBinary(const SourceLocation &srcLoc, const Token &op,
                      const std::shared_ptr<AST> &left,
                      const std::shared_ptr<AST> &right) noexcept {
  const std::shared_ptr<AST_BINARY> astBinary{
      std::make_shared<AST_BINARY>(generateNextId(), srcLoc, op, left, right)};
  if (left) {
    left->setFather(astBinary);
  }
  if (right) {
    right->setFather(astBinary);
  }
  return astBinary;
}

std::expected<std::shared_ptr<AST_UNARY>, Error>
Builder::createUnary(const SourceLocation &srcLoc, const Token &op,
                     const std::shared_ptr<AST> &value) noexcept {
  const std::shared_ptr<AST_UNARY> astUnary{
      std::make_shared<AST_UNARY>(generateNextId(), srcLoc, op, value)};
  if (value) {
    value->setFather(astUnary);
  }
  return astUnary;
}

std::expected<std::shared_ptr<AST_ASSIGNMENT>, Error>
Builder::createAssignment(const SourceLocation &srcLoc, const Token &op,
                          const std::shared_ptr<AST> &left,
                          const std::shared_ptr<AST> &value) noexcept {
  const std::shared_ptr<AST_ASSIGNMENT> astSelfAssignment{
      std::make_shared<AST_ASSIGNMENT>(generateNextId(), srcLoc, op, left,
                                       value)};
  if (left) {
    left->setFather(astSelfAssignment);
  }
  if (value) {
    value->setFather(astSelfAssignment);
  }
  return astSelfAssignment;
}

std::expected<std::shared_ptr<AST_PRINT>, Error>
Builder::createPrint(const SourceLocation &srcLoc,
                     const std::vector<std::shared_ptr<AST>> &values) noexcept {
  const std::shared_ptr<AST_PRINT> astPrint{
      std::make_shared<AST_PRINT>(generateNextId(), srcLoc, values)};
  const std::vector<std::shared_ptr<AST>> &values__{astPrint->values()};
  for (const std::shared_ptr<AST> &values_ : values__) {
    if (values_) {
      values_->setFather(astPrint);
    }
  }
  return astPrint;
}

std::expected<std::shared_ptr<AST_IMPORT>, Error>
Builder::createImport(const SourceLocation &srcLoc,
                      const std::filesystem::path &path) noexcept {
  const std::shared_ptr<AST_IMPORT> astImport{
      std::make_shared<AST_IMPORT>(generateNextId(), srcLoc, path)};
  return astImport;
}

std::expected<std::shared_ptr<AST_STATEMENT>, Error>
Builder::createStatement(const SourceLocation &srcLoc,
                         const std::shared_ptr<AST> &expression) noexcept {
  const std::shared_ptr<AST_STATEMENT> astStatement{
      std::make_shared<AST_STATEMENT>(generateNextId(), srcLoc, expression)};
  if (expression) {
    expression->setFather(astStatement);
  }
  return astStatement;
}

std::expected<std::shared_ptr<AST_BODY>, Error> Builder::createBody(
    const SourceLocation &srcLoc,
    const std::vector<std::shared_ptr<AST_STATEMENT>> &body) noexcept {
  const std::shared_ptr<AST_BODY> astBody{
      std::make_shared<AST_BODY>(generateNextId(), srcLoc, body)};
  const std::vector<std::shared_ptr<AST_STATEMENT>> &statements{
      astBody->body()};
  for (const std::shared_ptr<AST_STATEMENT> &statement : statements) {
    if (statement) {
      statement->setFather(astBody);
    }
  }
  return astBody;
}

std::expected<std::shared_ptr<AST_WHILE>, Error>
Builder::createWhile(const SourceLocation &srcLoc,
                     const std::shared_ptr<AST_CONDITION> &condition,
                     const std::shared_ptr<AST_BODY> &body) noexcept {
  const std::shared_ptr<AST_WHILE> astWhile{
      std::make_shared<AST_WHILE>(generateNextId(), srcLoc, condition, body)};
  if (condition) {
    condition->setFather(astWhile);
  }
  if (body) {
    body->setFather(astWhile);
  }
  return astWhile;
}

std::expected<std::shared_ptr<AST_FOR>, Error>
Builder::createFor(const SourceLocation &srcLoc,
                   const std::vector<std::shared_ptr<AST>> &init,
                   const std::shared_ptr<AST_CONDITION> &condition,
                   const std::vector<std::shared_ptr<AST>> &update,
                   const std::shared_ptr<AST_BODY> &body) noexcept {
  const std::shared_ptr<AST_FOR> astFor{std::make_shared<AST_FOR>(
      generateNextId(), srcLoc, init, condition, update, body)};
  const std::vector<std::shared_ptr<AST>> &init__{astFor->init()};
  for (const std::shared_ptr<AST> &init_ : init__) {
    if (init_) {
      astFor->setFather(init_);
    }
  }
  if (condition) {
    condition->setFather(astFor);
  }
  const std::vector<std::shared_ptr<AST>> &update__{astFor->update()};
  for (const std::shared_ptr<AST> &update_ : update__) {
    if (update_) {
      astFor->setFather(update_);
    }
  }
  if (body) {
    body->setFather(astFor);
  }
  return astFor;
}

std::expected<std::shared_ptr<AST_DO_WHILE>, Error> Builder::createDoWhile(
    const SourceLocation &srcLoc, const std::shared_ptr<AST_BODY> &body,
    const std::shared_ptr<AST_CONDITION> &condition) noexcept {
  const std::shared_ptr<AST_DO_WHILE> astDoWhile{std::make_shared<AST_DO_WHILE>(
      generateNextId(), srcLoc, body, condition)};
  if (condition) {
    condition->setFather(astDoWhile);
  }
  if (body) {
    body->setFather(astDoWhile);
  }
  return astDoWhile;
}

std::expected<std::shared_ptr<AST_PASS>, Error>
Builder::createPass(const SourceLocation &srcLoc) noexcept {
  const std::shared_ptr<AST_PASS> astPass{
      std::make_shared<AST_PASS>(generateNextId(), srcLoc)};
  return astPass;
}

std::expected<std::shared_ptr<AST_STOP>, Error>
Builder::createStop(const SourceLocation &srcLoc) noexcept {
  const std::shared_ptr<AST_STOP> astStop{
      std::make_shared<AST_STOP>(generateNextId(), srcLoc)};
  return astStop;
}

std::expected<std::shared_ptr<AST_IF>, Error>
Builder::createIf(const SourceLocation &srcLoc,
                  const std::shared_ptr<AST_CONDITION> &condition,
                  const std::shared_ptr<AST_BODY> &body,
                  const std::vector<std::shared_ptr<AST_ELSE_IF>> &elseIf,
                  const std::shared_ptr<AST_BODY> &elseBody) noexcept {
  const std::shared_ptr<AST_IF> astIf{std::make_shared<AST_IF>(
      generateNextId(), srcLoc, condition, body, elseIf, elseBody)};
  if (condition) {
    condition->setFather(astIf);
  }
  if (body) {
    body->setFather(astIf);
  }
  const std::vector<std::shared_ptr<AST_ELSE_IF>> &elseIfs{astIf->elseIf()};
  for (const std::shared_ptr<AST_ELSE_IF> &elseIf_ : elseIfs) {
    if (elseIf_) {
      elseIf_->setFather(astIf);
    }
  }
  if (elseBody) {
    elseBody->setFather(astIf);
  }
  return astIf;
}

std::expected<std::shared_ptr<AST_ELSE_IF>, Error>
Builder::createElseIf(const SourceLocation &srcLoc,
                      const std::shared_ptr<AST_CONDITION> &condition,
                      const std::shared_ptr<AST_BODY> &body) noexcept {
  const std::shared_ptr<AST_ELSE_IF> astIf{
      std::make_shared<AST_ELSE_IF>(generateNextId(), srcLoc, condition, body)};
  if (condition) {
    condition->setFather(astIf);
  }
  if (body) {
    body->setFather(astIf);
  }
  return astIf;
}

std::expected<std::shared_ptr<AST_SWITCH>, Error>
Builder::createSwitch(const SourceLocation &srcLoc,
                      const std::shared_ptr<AST_CONDITION> &condition,
                      const std::vector<std::shared_ptr<AST_CASE>> &cases,
                      const std::shared_ptr<AST_DEFAULT> &default_) noexcept {
  const std::shared_ptr<AST_SWITCH> astSwitch{std::make_shared<AST_SWITCH>(
      generateNextId(), srcLoc, condition, cases, default_)};
  if (condition) {
    condition->setFather(astSwitch);
  }
  const std::vector<std::shared_ptr<AST_CASE>> &cases_{astSwitch->cases()};
  for (const std::shared_ptr<AST_CASE> &statement : cases_) {
    if (statement) {
      statement->setFather(astSwitch);
    }
  }
  if (default_) {
    default_->setFather(astSwitch);
  }
  return astSwitch;
}

std::expected<std::shared_ptr<AST_CASE>, Error>
Builder::createCase(const SourceLocation &srcLoc,
                    const std::shared_ptr<AST> &match,
                    const std::shared_ptr<AST_BODY> &body) noexcept {
  const std::shared_ptr<AST_CASE> astCase{
      std::make_shared<AST_CASE>(generateNextId(), srcLoc, match, body)};
  if (match) {
    match->setFather(astCase);
  }
  if (body) {
    body->setFather(astCase);
  }
  return astCase;
}

std::expected<std::shared_ptr<AST_DEFAULT>, Error>
Builder::createDefault(const SourceLocation &srcLoc,
                       const std::shared_ptr<AST_BODY> &body) noexcept {
  const std::shared_ptr<AST_DEFAULT> astDefault{
      std::make_shared<AST_DEFAULT>(generateNextId(), srcLoc, body)};
  if (body) {
    body->setFather(astDefault);
  }
  return astDefault;
}

std::expected<std::shared_ptr<AST_TERNARY>, Error>
Builder::createTernary(const SourceLocation &srcLoc,
                       const std::shared_ptr<AST_CONDITION> &condition,
                       const std::shared_ptr<AST> &first,
                       const std::shared_ptr<AST> &second) noexcept {
  const std::shared_ptr<AST_TERNARY> astTernanry{std::make_shared<AST_TERNARY>(
      generateNextId(), srcLoc, condition, first, second)};
  if (condition) {
    condition->setFather(astTernanry);
  }
  if (first) {
    first->setFather(astTernanry);
  }
  if (second) {
    second->setFather(astTernanry);
  }
  return astTernanry;
}

std::expected<std::shared_ptr<AST_CONDITION>, Error>
Builder::createCondition(const SourceLocation &srcLoc,
                         const std::shared_ptr<AST> &expression) noexcept {
  const std::shared_ptr<AST_CONDITION> astCondition{
      std::make_shared<AST_CONDITION>(generateNextId(), srcLoc, expression)};
  if (expression) {
    expression->setFather(astCondition);
  }
  return astCondition;
}

std::expected<std::shared_ptr<AST_FUNC_CALL>, Error> Builder::createFunCall(
    const SourceLocation &srcLoc, const std::string &id,
    const std::vector<std::shared_ptr<Type>> &replaceOfGenerics,
    const std::vector<std::shared_ptr<AST>> &parameters) noexcept {
  const std::shared_ptr<AST_FUNC_CALL> astFuncCall{
      std::make_shared<AST_FUNC_CALL>(generateNextId(), srcLoc, id,
                                      replaceOfGenerics, parameters)};
  const std::vector<std::shared_ptr<AST>> &parameters__{
      astFuncCall->parameters()};
  for (const std::shared_ptr<AST> &parameters_ : parameters__) {
    if (parameters_) {
      parameters_->setFather(astFuncCall);
    }
  }
  return astFuncCall;
}

std::expected<std::shared_ptr<AST_FUNC_DECL>, Error>
Builder::createFuncDecl(const SourceLocation &srcLoc, const std::string &id,
                        const std::vector<GenericParameter> &generics,
                        const Parameters &params,
                        const std::shared_ptr<Type> &returnType,
                        const std::shared_ptr<AST_BODY> &body) noexcept {
  const std::shared_ptr<AST_FUNC_DECL> astFuncDecl{
      std::make_shared<AST_FUNC_DECL>(generateNextId(), srcLoc, id, generics,
                                      params, returnType, body)};
  if (body) {
    body->setFather(astFuncDecl);
  }
  return astFuncDecl;
}

std::expected<std::shared_ptr<AST_RETURN>, Error>
Builder::createReturn(const SourceLocation &srcLoc,
                      const std::shared_ptr<AST> &value) noexcept {
  const std::shared_ptr<AST_RETURN> astReturn{
      std::make_shared<AST_RETURN>(generateNextId(), srcLoc, value)};
  if (value) {
    value->setFather(astReturn);
  }
  return astReturn;
}

std::expected<std::shared_ptr<AST_ENUM>, Error>
Builder::createEnum(const SourceLocation &srcLoc, const std::string &id,
                    const std::vector<std::string> &enumIdentifiers) noexcept {
  return std::make_shared<AST_ENUM>(generateNextId(), srcLoc, id,
                                    enumIdentifiers);
}

std::expected<std::shared_ptr<AST_ENUM_ACCESS>, Error>
Builder::createEnumAccess(const SourceLocation &srcLoc, const std::string &id,
                          const std::string &identifiers) noexcept {
  return std::make_shared<AST_ENUM_ACCESS>(generateNextId(), srcLoc, id,
                                           identifiers);
}

std::expected<std::shared_ptr<AST_STRUCT>, Error> Builder::createStruct(
    const SourceLocation &srcLoc, const std::string &id,
    const std::vector<GenericParameter> &generics,
    const std::shared_ptr<Type> &fatherType, const Attributes &attributes,
    const std::vector<std::shared_ptr<AST_METHOD_DECL>> &methods,
    const std::shared_ptr<AST_CONSTRUCTOR_DECL> &constructor,
    const std::shared_ptr<AST_DESTRUCTOR_DECL> &destructor) noexcept {
  const std::shared_ptr<AST_STRUCT> astStruct{std::make_shared<AST_STRUCT>(
      generateNextId(), srcLoc, id, generics, fatherType, attributes, methods,
      constructor, destructor)};
  const std::vector<std::shared_ptr<AST_METHOD_DECL>> &methods__{
      astStruct->methods()};
  for (const std::shared_ptr<AST_METHOD_DECL> &methods_ : methods__) {
    if (methods_) {
      methods_->setFather(astStruct);
    }
  }
  if (constructor) {
    constructor->setFather(astStruct);
  }
  if (destructor) {
    destructor->setFather(astStruct);
  }
  return astStruct;
}

std::expected<std::shared_ptr<AST_ATTR_ACCESS>, Error>
Builder::createAttrAccess(const SourceLocation &srcLoc,
                          const std::string &id) noexcept {
  return std::make_shared<AST_ATTR_ACCESS>(generateNextId(), srcLoc, id);
}

std::expected<std::shared_ptr<AST_METHOD_CALL>, Error>
Builder::createMethodCall(
    const SourceLocation &srcLoc, const std::string &id,
    const std::vector<std::shared_ptr<Type>> &replaceOfGenerics,
    const std::vector<std::shared_ptr<AST>> &parameters) noexcept {
  const std::shared_ptr<AST_METHOD_CALL> astMethodCall{
      std::make_shared<AST_METHOD_CALL>(generateNextId(), srcLoc, id,
                                        replaceOfGenerics, parameters)};
  const std::vector<std::shared_ptr<AST>> &parameters__{
      astMethodCall->parameters()};
  for (const std::shared_ptr<AST> &parameters_ : parameters__) {
    if (parameters_) {
      parameters_->setFather(astMethodCall);
    }
  }
  return astMethodCall;
}

std::expected<std::shared_ptr<AST_METHOD_DECL>, Error>
Builder::createMethodDecl(const SourceLocation &srcLoc, const std::string &id,
                          const std::vector<GenericParameter> &generics,
                          const Parameters &params,

                          const std::shared_ptr<Type> &returnType,
                          const std::shared_ptr<AST_BODY> &body,
                          const bool isVirtual) noexcept {
  const std::shared_ptr<AST_METHOD_DECL> astMethodDecl{
      std::make_shared<AST_METHOD_DECL>(generateNextId(), srcLoc, id, generics,
                                        params, returnType, body, isVirtual)};
  if (body) {
    body->setFather(astMethodDecl);
  }
  return astMethodDecl;
}

std::expected<std::shared_ptr<AST_CONSTRUCTOR_CALL>, Error>
Builder::createConstructorCall(
    const SourceLocation &srcLoc, const std::string &id,
    const std::vector<std::shared_ptr<Type>> &replaceOfGenerics,
    const std::vector<std::shared_ptr<AST>> &parameters) noexcept {
  const std::shared_ptr<AST_CONSTRUCTOR_CALL> astConstructorall{
      std::make_shared<AST_CONSTRUCTOR_CALL>(generateNextId(), srcLoc, id,
                                             replaceOfGenerics, parameters)};
  const std::vector<std::shared_ptr<AST>> &parameters__{
      astConstructorall->parameters()};
  for (const std::shared_ptr<AST> &parameters_ : parameters__) {
    if (parameters_) {
      parameters_->setFather(astConstructorall);
    }
  }
  return astConstructorall;
}

std::expected<std::shared_ptr<AST_CONSTRUCTOR_DECL>, Error>
Builder::createConstructorDecl(const SourceLocation &srcLoc,
                               const std::string &id,
                               const std::vector<GenericParameter> &generics,
                               const Parameters &params,
                               const std::shared_ptr<AST_SUPER> &super,
                               const std::shared_ptr<Type> &returnType,
                               const std::shared_ptr<AST_BODY> &body) noexcept {
  const std::shared_ptr<AST_CONSTRUCTOR_DECL> astConstructor{
      std::make_shared<AST_CONSTRUCTOR_DECL>(generateNextId(), srcLoc, id,
                                             generics, params, super,
                                             returnType, body)};
  if (super) {
    super->setFather(astConstructor);
  }
  if (body) {
    body->setFather(astConstructor);
  }
  return astConstructor;
}

std::expected<std::shared_ptr<AST_SUPER>, Error> Builder::createSuper(
    const SourceLocation &srcLoc, const std::shared_ptr<Type> &fatherType,
    const std::vector<std::shared_ptr<Type>> &replacements,
    const std::vector<std::shared_ptr<AST>> &arguments) noexcept {
  const std::shared_ptr<AST_SUPER> astSuper{std::make_shared<AST_SUPER>(
      generateNextId(), srcLoc, fatherType, replacements, arguments)};
  const std::vector<std::shared_ptr<AST>> &values__{astSuper->arguments()};
  for (const std::shared_ptr<AST> &values_ : values__) {
    if (values_) {
      values_->setFather(astSuper);
    }
  }
  return astSuper;
}

std::expected<std::shared_ptr<AST_DESTRUCTOR_DECL>, Error>
Builder::createDestructorDecl(const SourceLocation &srcLoc,
                              const std::string &id,
                              const std::shared_ptr<Type> &returnType,
                              const std::shared_ptr<AST_BODY> &body,
                              const bool isVirtual) noexcept {
  const std::shared_ptr<AST_DESTRUCTOR_DECL> astConstructor{
      std::make_shared<AST_DESTRUCTOR_DECL>(generateNextId(), srcLoc, id,
                                            returnType, body, isVirtual)};
  if (body) {
    body->setFather(astConstructor);
  }
  return astConstructor;
}

std::expected<std::shared_ptr<AST_THIS>, Error>
Builder::createThis(const SourceLocation &srcLoc) noexcept {
  const std::shared_ptr<AST_THIS> astThis{
      std::make_shared<AST_THIS>(generateNextId(), srcLoc)};
  return astThis;
}

std::expected<std::shared_ptr<AST_AUTO_DECL>, Error>
Builder::createAutoDecl(const SourceLocation &srcLoc, const std::string &id,
                        const std::shared_ptr<AST> &value,
                        const bool isConst) noexcept {
  const std::shared_ptr<AST_AUTO_DECL> astAutoDecl{
      std::make_shared<AST_AUTO_DECL>(generateNextId(), srcLoc, id, value,
                                      isConst)};
  if (value) {
    value->setFather(astAutoDecl);
  }
  return astAutoDecl;
}

std::expected<std::shared_ptr<AST_VAR_TYPED_DECL>, Error>
Builder::createVarTypedtDecl(const SourceLocation &srcLoc,
                             const std::string &id,
                             const std::shared_ptr<Type> &type,
                             const std::shared_ptr<AST> &value) noexcept {
  const std::shared_ptr<AST_VAR_TYPED_DECL> astLetDecl{
      std::make_shared<AST_VAR_TYPED_DECL>(generateNextId(), srcLoc, id, type,
                                           value)};
  if (value) {
    value->setFather(astLetDecl);
  }
  return astLetDecl;
}

std::expected<std::shared_ptr<AST_VAR_CALL>, Error>
Builder::createVarCall(const SourceLocation &srcLoc,
                       const std::string &id) noexcept {
  return std::make_shared<AST_VAR_CALL>(generateNextId(), srcLoc, id);
}

std::expected<std::shared_ptr<AST_CHAINED>, Error> Builder::createChained(
    const SourceLocation &srcLoc, const std::shared_ptr<AST> &base,
    const std::vector<std::shared_ptr<AST>> &operations) noexcept {
  const std::shared_ptr<AST_CHAINED> astChained{std::make_shared<AST_CHAINED>(
      generateNextId(), srcLoc, base, operations)};
  if (base) {
    base->setFather(astChained);

    const std::vector<std::shared_ptr<AST>> &operations__{
        astChained->operations()};

    if (operations__.size()) {
      operations__[0]->setFather(base);

      const std::size_t size{operations__.size()};
      if (size > 1) {
        for (std::size_t i{1}; i < size; ++i) {
          if (operations__[i] and operations__[i - 1]) {
            operations__[i]->setFather(operations__[i - 1]);
          }
        }
      }
    }
  }
  return astChained;
}

std::expected<std::shared_ptr<Tree>, Error>
Builder::createTree(const std::shared_ptr<AST_BODY> &statements) noexcept {
  return std::make_shared<Tree>(statements);
}

} // namespace nicole