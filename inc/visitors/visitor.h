#ifndef VISITOR_H
#define VISITOR_H

#include "../errors.h"
#include <expected>
#include <memory>

namespace nicole {

// Forward declaration to avoid circular dependency
enum class TokenType;

class AST;

class AST_BOOL;
class AST_CHAR;
class AST_DOUBLE;
class AST_FLOAT;
class AST_INT;
class AST_NULL;
class AST_STRING;

class AST_VECTOR;
class AST_INDEX;

class AST_DELETE;
class AST_NEW;
class AST_DEREF;

class AST_BINARY;

class AST_UNARY;

class AST_ASSIGNMENT;

class AST_PRINT;
class AST_IMPORT;

class AST_STATEMENT;
class AST_BODY;

class AST_WHILE;
class AST_FOR;
class AST_DO_WHILE;
class AST_PASS;
class AST_STOP;

class AST_IF;
class AST_ELSE_IF;
class AST_SWITCH;
class AST_CASE;
class AST_DEFAULT;
class AST_TERNARY;
class AST_CONDITION;

class AST_FUNC_CALL;
class AST_FUNC_DECL;
class AST_RETURN;

class AST_ENUM;
class AST_ENUM_ACCESS;

class AST_STRUCT;
class AST_ATTR_ACCESS;
class AST_METHOD_CALL;
class AST_THIS;
class AST_CONSTRUCTOR_CALL;
class AST_METHOD_DECL;
class AST_CONSTRUCTOR_DECL;
class AST_DESTRUCTOR_DECL;
class AST_SUPER;

class AST_AUTO_DECL;
class AST_VAR_TYPED_DECL;
class AST_VAR_CALL;

class AST_CHAINED;

class Tree;

// Abstract class that represents the visitor pattern
template <class T>
class Visitor : public std::enable_shared_from_this<Visitor<T>> {
public:
  virtual ~Visitor() = default;

  [[nodiscard]] virtual auto visit(const AST_BOOL *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_CHAR *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_DOUBLE *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_FLOAT *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_INT *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_NULL *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_STRING *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_VECTOR *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_INDEX *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_DELETE *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_NEW *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_DEREF *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_BINARY *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_UNARY *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_ASSIGNMENT *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_PRINT *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_IMPORT *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_STATEMENT *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_BODY *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_WHILE *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_FOR *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_DO_WHILE *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_PASS *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_STOP *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_IF *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_ELSE_IF *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_SWITCH *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_CASE *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_DEFAULT *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_TERNARY *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_CONDITION *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_FUNC_CALL *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_FUNC_DECL *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_RETURN *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_ENUM *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_ENUM_ACCESS *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_STRUCT *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_ATTR_ACCESS *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_METHOD_CALL *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_THIS *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto
  visit(const AST_CONSTRUCTOR_CALL *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_METHOD_DECL *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto
  visit(const AST_CONSTRUCTOR_DECL *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_SUPER *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto
  visit(const AST_DESTRUCTOR_DECL *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_AUTO_DECL *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto
  visit(const AST_VAR_TYPED_DECL *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_VAR_CALL *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const AST_CHAINED *node) const noexcept
      -> std::expected<T, Error> = 0;

  [[nodiscard]] virtual auto visit(const Tree *tree) const noexcept
      -> std::expected<T, Error> = 0;
};

} // namespace nicole

#endif