#ifndef AST_TYPE_H
#define AST_TYPE_H

#include <string>

namespace nicole {

enum class AST_TYPE : int {
  EXPRESSION,

  // Literals
  BOOL,
  INT,
  CHAR,
  DOUBLE,
  FLOAT,
  STRING,
  NULLPTR,

  // Vectors
  VECTOR,
  INDEX,

  // Pointers
  PTR,
  PTR_DEREFENCE,
  NEW,
  DELETE,

  // Binary operators
  BINARY,

  // Unary operators
  UNARY,

  // Variable declaration
  VAR_TYPED_DECL,
  AUTO_DECL,
  VAR_CALL,

  // Asignment
  ASSIGNMENT,

  // Utils
  PRINT,
  IMPORT,

  // Statements
  STATEMENT,
  BODY,

  // Loops
  WHILE,
  FOR,
  DO_WHILE,
  PASS,
  STOP,

  // Conditionals
  IF,
  ELSE_IF,
  SWITCH,
  CASE,
  DEFAULT,
  TERNARY,
  CONDITION,

  // Functions
  FUNC_DECL,
  PARAMETER,
  RETURN,
  FUN_CALL,

  // Structs
  STRUCT_DECL,

  // Classes
  CLASS_DECL,

  METHOD_CALL,
  ATTR_ACCESS,
  THIS,
  CONSTRUCTOR_CALL,
  METHOD_DECL,
  CONSTRUCTOR_DECL,
  DESTRUCTOR_DECL,
  SUPER,

  // Enum
  ENUM,
  ENUM_ACCESS,

  // Chained expression
  CHAIN

};

[[nodiscard]] auto astTypeToStr(const AST_TYPE type) noexcept -> std::string;

} // namespace nicole

#endif