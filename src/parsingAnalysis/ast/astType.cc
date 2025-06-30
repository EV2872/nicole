#include "../../../inc/parsingAnalysis/ast/astType.h"

namespace nicole {

auto astTypeToStr(const AST_TYPE type) noexcept -> std::string {
  switch (type) {
  case AST_TYPE::EXPRESSION:
    return "EXPRESSION";

    // Literals
  case AST_TYPE::BOOL:
    return "BOOL";
  case AST_TYPE::INT:
    return "INT";
  case AST_TYPE::CHAR:
    return "CHAR";
  case AST_TYPE::DOUBLE:
    return "DOUBLE";
  case AST_TYPE::FLOAT:
    return "FLOAT";
  case AST_TYPE::STRING:
    return "STRING";
  case AST_TYPE::NULLPTR:
    return "NULLPTR";

    // Vectors
  case AST_TYPE::VECTOR:
    return "VECTOR";
  case AST_TYPE::INDEX:
    return "INDEX";

    // Pointers
  case AST_TYPE::PTR:
    return "PTR";
  case AST_TYPE::PTR_DEREFENCE:
    return "PTR_DEREFENCE";
  case AST_TYPE::NEW:
    return "NEW";
  case AST_TYPE::DELETE:
    return "DELETE";

    // Binary operators
  case AST_TYPE::BINARY:
    return "BINARY";

  // Unary operators
  case AST_TYPE::UNARY:
    return "UNARY";

    // Variable declaration
  case AST_TYPE::VAR_TYPED_DECL:
    return "VAR_TYPED_DECL";
  case AST_TYPE::AUTO_DECL:
    return "AUTO_DECL";
  case AST_TYPE::VAR_CALL:
    return "VAR_CALL";

    // Asignment
  case AST_TYPE::ASSIGNMENT:
    return "ASIGNMENT";

    // Utils
  case AST_TYPE::PRINT:
    return "PRINT";
  case AST_TYPE::IMPORT:
    return "IMPORT";

    // Statements
  case AST_TYPE::STATEMENT:
    return "STATEMENT";
  case AST_TYPE::BODY:
    return "BODY";

    // Loops
  case AST_TYPE::WHILE:
    return "WHILE";
  case AST_TYPE::FOR:
    return "FOR";
  case AST_TYPE::DO_WHILE:
    return "DO_WHILE";
  case AST_TYPE::PASS:
    return "PASS";
  case AST_TYPE::STOP:
    return "STOP";

    // Conditionals
  case AST_TYPE::IF:
    return "IF";
  case AST_TYPE::ELSE_IF:
    return "ELSE_IF";
  case AST_TYPE::SWITCH:
    return "SWITCH";
  case AST_TYPE::CASE:
    return "CASE";
  case AST_TYPE::DEFAULT:
    return "DEFAULT";
  case AST_TYPE::TERNARY:
    return "TERNARY";
  case AST_TYPE::CONDITION:
    return "CONDITION";

    // Functions
  case AST_TYPE::FUNC_DECL:
    return "FUNC_DECL";
  case AST_TYPE::PARAMETER:
    return "PARAMETER";
  case AST_TYPE::RETURN:
    return "RETURN";
  case AST_TYPE::FUN_CALL:
    return "FUN_CALL";

    // Structs
  case AST_TYPE::STRUCT_DECL:
    return "STRUCT_DECL";

    // Classes
  case AST_TYPE::CLASS_DECL:
    return "CLASS_DECL";

  case AST_TYPE::ATTR_ACCESS:
    return "ATTR_ACCESS";

  case AST_TYPE::METHOD_CALL:
    return "METHOD_CALL";

  case AST_TYPE::METHOD_DECL:
    return "METHOD_DECL";

  case AST_TYPE::CONSTRUCTOR_DECL:
    return "CONSTRUCTOR_DECL";

  case AST_TYPE::DESTRUCTOR_DECL:
    return "DESTRUCTOR_DECL";

  case AST_TYPE::THIS:
    return "THIS";

  case AST_TYPE::CONSTRUCTOR_CALL:
    return "CONSTRUCTOR_CALL";

  case AST_TYPE::SUPER:
    return "SUPER";

    // Enum
  case AST_TYPE::ENUM:
    return "ENUM";

  case AST_TYPE::ENUM_ACCESS:
    return "ENUM_ACCESS";

  // Chained expression
  case AST_TYPE::CHAIN:
    return "CHAIN";
  }

  return "UNDEFINED_TYPE";
}

} // namespace nicole