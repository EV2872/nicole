#ifndef TYPE_H
#define TYPE_H

#include <string>

namespace nicole {

// We use a enum so the parsing doesnt relly into knowing the raw data or a
// string comparasion between types which is slower
enum class TokenType {
  SPACE,
  COMMENT,

  NUMBER_FLOAT,
  NUMBER_DOUBLE,
  NUMBER_INT,
  CHAR,
  STRING,
  TRUE,
  FALSE,
  NULLPTR,

  TYPE_VOID,
  TYPE_INT,
  TYPE_BOOL,
  TYPE_CHAR,
  TYPE_FLOAT,
  TYPE_DOUBLE,
  TYPE_STR,

  LET,
  CONST,
  AUTO,
  PTR,

  VECTOR,

  NEW,
  DELETE,

  IMPORT,
  PRINT,
  SYSTEM,
  TYPE,

  IF,
  ELSE,
  SWITCH,
  CASE,
  DEFAULT,

  WHILE,
  FOR,
  DO,
  STOP,
  PASS,

  RETURN,
  FUNCTION,

  ENUM,
  STRUCT,
  CLASS,
  PRIVATE,
  PUBLIC,
  PROTECTED,
  EXTENDS,
  METHOD,
  ATTR,
  THIS,
  OVERRIDE,
  CONSTRUCTOR,
  DESTRUCTOR,
  SUPER,
  VIRTUAL,

  OR,
  AND,

  ID,

  RP,
  LP,
  RB,
  LB,
  RC,
  LC,

  SEMICOLON,

  COMMA,

  TERNARY,

  DOT,
  DOTDOT,

  SMALLEREQUAL,
  BIGGEREQUAL,
  NOTEQUAL,
  EQUAL,
  ASSIGNMENT,

  INCREMENT,
  DECREMENT,

  OPERATOR_ADD,
  OPERATOR_SUB,
  OPERATOR_MULT,
  OPERATOR_DIV,
  OPERATOR_SMALLER,
  OPERATOR_GREATER,
  OPERATOR_NOT,
  OPERATOR_MODULE,

  SELF_ADD,
  SELF_SUB,
  SELF_MULT,
  SELF_DIV,
  DOTDOT_ASSIGNMENT,

  MAIN,

  UNMATCHED, // Para tokens no reconocidos
  ALL
};

[[nodiscard]] auto tokenTypeToString(const TokenType &tk) noexcept
    -> std::string;

} // namespace nicole

#endif