#ifndef NICOLE_ERRORS_H
#define NICOLE_ERRORS_H

#include <expected>
#include <ostream>

namespace nicole {

enum class ERROR_TYPE {
  // Options
  MISSING_ARGUMENTS,
  MISSING_BINARY_NAME,
  MISSING_ENTRY_FILE,
  ENTRY_FILE_NOT_FOUND,
  UNKOWN_ARGUMENT,

  // Lexer
  CHECK_UNMATCHED,
  FILE_EXTENSION,
  FILE_NOT_OPEN,

  // TokeStream
  EAT,
  CURRENT,
  LOOK_AHEAD,
  LAST_READ,
  IS_CURRENT_TOKEN_TYPE,
  INSERT_AFTER,

  BUILDER,

  SINTAX,

  NULL_NODE,

  ENUM,
  VARIABLE,
  TYPE,
  FUNCTION,
  METHOD,
  ATTR,

  PRINT_TREE,
  VALIDATE_TREE
};

[[nodiscard]] inline auto errorToStr(const ERROR_TYPE type) noexcept
    -> std::string {
  switch (type) {
  // Options
  case ERROR_TYPE::MISSING_ARGUMENTS:
    return "MISSING_ARGUMENTS";
  case ERROR_TYPE::MISSING_BINARY_NAME:
    return "MISSING_BINARY_NAME";
  case ERROR_TYPE::MISSING_ENTRY_FILE:
    return "MISSING_ENTRY_FILE";
  case ERROR_TYPE::ENTRY_FILE_NOT_FOUND:
    return "ENTRY_FILE_NOT_FOUND";
  case ERROR_TYPE::UNKOWN_ARGUMENT:
    return "UNKOWN_ARGUMENT";

  // Lexer
  case ERROR_TYPE::CHECK_UNMATCHED:
    return "CHECK_UNMATCHED";
  case ERROR_TYPE::FILE_EXTENSION:
    return "FILE_EXTENSION";
  case ERROR_TYPE::FILE_NOT_OPEN:
    return "FILE_NOT_OPEN";

  // TokeStream
  case ERROR_TYPE::EAT:
    return "EAT";
  case ERROR_TYPE::CURRENT:
    return "CURRENT";
  case ERROR_TYPE::LOOK_AHEAD:
    return "LOOK_AHEAD";
  case ERROR_TYPE::LAST_READ:
    return "LAST_READ";
  case ERROR_TYPE::IS_CURRENT_TOKEN_TYPE:
    return "IS_CURRENT_TOKEN_TYPE";
  case ERROR_TYPE::INSERT_AFTER:
    return "INSERT_AFTER";

  case ERROR_TYPE::BUILDER:
    return "BUILDER";

  case ERROR_TYPE::SINTAX:
    return "SINTAX";

  case ERROR_TYPE::NULL_NODE:
    return "NULL_NODE";

  case ERROR_TYPE::ENUM:
    return "ENUM";

  case ERROR_TYPE::TYPE:
    return "TYPE";

  case ERROR_TYPE::VARIABLE:
    return "VARIABLE";

  case ERROR_TYPE::FUNCTION:
    return "FUNCTION";

  case ERROR_TYPE::METHOD:
    return "METHOD";

  case ERROR_TYPE::ATTR:
    return "ATTR";

  case ERROR_TYPE::PRINT_TREE:
    return "PRINT_TREE";

  case ERROR_TYPE::VALIDATE_TREE:
    return "VALIDATE_TREE";
  }
}

class Error final {
private:
  ERROR_TYPE type_;
  std::string info_;

public:
  explicit Error(const ERROR_TYPE type, const std::string &info) noexcept
      : type_{type}, info_{info} {}

  [[nodiscard]] auto type() const noexcept -> ERROR_TYPE { return type_; }

  [[nodiscard]] auto info() const noexcept -> const std::string & {
    return info_;
  }

  friend auto operator<<(std::ostream &os, const Error &error) noexcept
      -> std::ostream & {
    return os << "Error " << errorToStr(error.type_) << ": " << error.info_;
  }
};

[[nodiscard]] inline auto createError(const ERROR_TYPE type,
                                      const std::string &info) noexcept
    -> std::unexpected<Error> {
  return std::unexpected{Error{type, info}};
}

[[nodiscard]] inline auto createError(const Error &error) noexcept
    -> std::unexpected<Error> {
  return std::unexpected{error};
}

} // namespace nicole

#endif