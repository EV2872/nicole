#include "../../inc/lexicalAnalysis/type.h"

namespace nicole {

auto tokenTypeToString(const TokenType &tk) noexcept -> std::string {
  switch (tk) {
  case TokenType::SPACE:
    return "SPACE";
  case TokenType::COMMENT:
    return "COMMENT";

  case TokenType::NUMBER_FLOAT:
    return "NUMBER_FLOAT";
  case TokenType::NUMBER_DOUBLE:
    return "NUMBER_DOUBLE";
  case TokenType::NUMBER_INT:
    return "NUMBER_INT";
  case TokenType::CHAR:
    return "CHAR";
  case TokenType::STRING:
    return "STRING";
  case TokenType::TRUE:
    return "TRUE";
  case TokenType::FALSE:
    return "FALSE";
  case TokenType::NULLPTR:
    return "NULLPTR";

  case TokenType::TYPE_VOID:
    return "TYPE_VOID";
  case TokenType::TYPE_INT:
    return "TYPE_INT";
  case TokenType::TYPE_BOOL:
    return "TYPE_BOOL";
  case TokenType::TYPE_CHAR:
    return "TYPE_CHAR";
  case TokenType::TYPE_FLOAT:
    return "TYPE_FLOAT";
  case TokenType::TYPE_DOUBLE:
    return "TYPE_DOUBLE";
  case TokenType::TYPE_STR:
    return "TYPE_STR";

  case TokenType::LET:
    return "LET";
  case TokenType::CONST:
    return "CONST";
  case TokenType::AUTO:
    return "AUTO";
  case TokenType::PTR:
    return "PTR";

  case TokenType::VECTOR:
    return "VECTOR";

  case TokenType::NEW:
    return "NEW";
  case TokenType::DELETE:
    return "DELETE";

  case TokenType::IMPORT:
    return "IMPORT";
  case TokenType::PRINT:
    return "PRINT";
  case TokenType::SYSTEM:
    return "SYSTEM";
  case TokenType::TYPE:
    return "TYPE";

  case TokenType::IF:
    return "IF";
  case TokenType::ELSE:
    return "ELSE";
  case TokenType::SWITCH:
    return "SWITCH";
  case TokenType::CASE:
    return "CASE";
  case TokenType::DEFAULT:
    return "DEFAULT";

  case TokenType::WHILE:
    return "WHILE";
  case TokenType::FOR:
    return "FOR";
  case TokenType::DO:
    return "DO";
  case TokenType::STOP:
    return "STOP";
  case TokenType::PASS:
    return "PASS";

  case TokenType::RETURN:
    return "RETURN";
  case TokenType::FUNCTION:
    return "FUNCTION";

  case TokenType::ENUM:
    return "ENUM";
  case TokenType::STRUCT:
    return "STRUCT";
  case TokenType::CLASS:
    return "CLASS";
  case TokenType::PRIVATE:
    return "PRIVATE";
  case TokenType::PUBLIC:
    return "PUBLIC";
  case TokenType::PROTECTED:
    return "PROTECTED";
  case TokenType::EXTENDS:
    return "EXTENDS";
  case TokenType::METHOD:
    return "METHOD";
  case TokenType::ATTR:
    return "ATTR";
  case TokenType::THIS:
    return "THIS";
  case TokenType::OVERRIDE:
    return "OVERRIDE";
  case TokenType::CONSTRUCTOR:
    return "CONSTRUCTOR";
  case TokenType::DESTRUCTOR:
    return "DESTRUCTOR";
  case TokenType::SUPER:
    return "SUPER";
  case TokenType::VIRTUAL:
    return "VIRTUAL";

  case TokenType::OR:
    return "OR";
  case TokenType::AND:
    return "AND";

  case TokenType::ID:
    return "ID";

  case TokenType::RP:
    return "RP"; // Right Parenthesis ')'
  case TokenType::LP:
    return "LP"; // Left Parenthesis '('
  case TokenType::RB:
    return "RB"; // Right Bracket ']'
  case TokenType::LB:
    return "LB"; // Left Bracket '['
  case TokenType::RC:
    return "RC"; // Right Curly Brace '}'
  case TokenType::LC:
    return "LC"; // Left Curly Brace '{'

  case TokenType::SEMICOLON:
    return "SEMICOLON";

  case TokenType::COMMA:
    return "COMMA";

  case TokenType::TERNARY:
    return "TERNARY";

  case TokenType::DOT:
    return "DOT";
  case TokenType::DOTDOT:
    return "DOTDOT"; // :

  case TokenType::SMALLEREQUAL:
    return "SMALLEREQUAL";
  case TokenType::BIGGEREQUAL:
    return "BIGGEREQUAL";
  case TokenType::NOTEQUAL:
    return "NOTEQUAL";
  case TokenType::EQUAL:
    return "EQUAL";
  case TokenType::ASSIGNMENT:
    return "ASSIGNMENT"; // '='

  case TokenType::INCREMENT:
    return "INCREMENT"; // '++'
  case TokenType::DECREMENT:
    return "DECREMENT"; // '--'

  case TokenType::OPERATOR_ADD:
    return "OPERATOR_ADD"; // '+'
  case TokenType::OPERATOR_SUB:
    return "OPERATOR_SUB"; // '-'
  case TokenType::OPERATOR_MULT:
    return "OPERATOR_MULT"; // '*'
  case TokenType::OPERATOR_DIV:
    return "OPERATOR_DIV"; // '/'
  case TokenType::OPERATOR_SMALLER:
    return "OPERATOR_SMALLER"; // '<'
  case TokenType::OPERATOR_GREATER:
    return "OPERATOR_GREATER"; // '>'
  case TokenType::OPERATOR_NOT:
    return "OPERATOR_NOT"; // '!'
  case TokenType::OPERATOR_MODULE:
    return "OPERATOR_MODULE"; // '%'

  case TokenType::SELF_ADD:
    return "SELF_ADD";
  case TokenType::SELF_SUB:
    return "SELF_SUB";
  case TokenType::SELF_MULT:
    return "SELF_MULT";
  case TokenType::SELF_DIV:
    return "SELF_DIV";
  case TokenType::DOTDOT_ASSIGNMENT:
    return "DOTDOT_ASSIGNMENT";

  case TokenType::MAIN:
    return "SELF_DIV";

  case TokenType::UNMATCHED:
    return "UNMATCHED"; // For unrecognized tokens
  case TokenType::ALL:
    return "ALL"; // Catch-all case for all tokens
  default:
    return "UNKNOWN"; // In case of an unrecognized token type
  }
}

} // namespace nicole
