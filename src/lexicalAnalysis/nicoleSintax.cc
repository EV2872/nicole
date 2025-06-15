#include "../../inc/lexicalAnalysis/nicoleSintax.h"

namespace nicole {

auto NicoleSintax::createLexer() const noexcept -> Lexer {
  return Lexer{
      std::vector<Category>{
          Category{TokenType::SPACE, "\\s+", true},
          Category{TokenType::COMMENT, "\\/\\*(.|\\n)*?\\*\\/", true},

          Category{TokenType::NUMBER_FLOAT, "f\\d+\\.\\d+([eE][-]?\\d+)?",
                   false},
          Category{TokenType::NUMBER_DOUBLE, "\\d+\\.\\d+([eE][-]?\\d+)?",
                   false},
          Category{TokenType::NUMBER_INT, "\\d+([eE][-]?\\d+)?", false},
          Category{TokenType::CHAR, "\'(?:\\\\.|[^\'\\\\])\'", false},
          Category{TokenType::STRING, "\"(?:\\\\.|[^\"\\\\])*\"", false},
          Category{TokenType::TRUE, "\\btrue\\b", false},
          Category{TokenType::FALSE, "\\bfalse\\b", false},
          Category{TokenType::NULLPTR, "\\bnull\\b", false},

          Category{TokenType::TYPE_VOID, "\\bvoid\\b", false},
          Category{TokenType::TYPE_BOOL, "\\bbool\\b", false},
          Category{TokenType::TYPE_CHAR, "\\bchar\\b", false},
          Category{TokenType::TYPE_DOUBLE, "\\bdouble\\b", false},
          Category{TokenType::TYPE_FLOAT, "\\bfloat\\b", false},
          Category{TokenType::TYPE_INT, "\\bint\\b", false},

          // constructor here to avoid conflict eith const
          Category{TokenType::CONSTRUCTOR, "\\bconstructor\\b", false},
          Category{TokenType::DESTRUCTOR, "\\bdestructor\\b", false},

          Category{TokenType::CONST, "\\bconst\\b", false},
          Category{TokenType::LET, "\\blet\\b", false},
          Category{TokenType::AUTO, "\\bauto\\b", false},
          Category{TokenType::PTR, "ptr", false},
          Category{TokenType::VECTOR, "\\bvector\\b", false},

          Category{TokenType::NEW, "\\bnew\\b", false},
          Category{TokenType::DELETE, "\\bdelete\\b", false},

          Category{TokenType::IMPORT, "\\bimport\\b", false},
          Category{TokenType::PRINT, "\\bprint\\b", false},
          Category{TokenType::SYSTEM, "\\bsystem\\b", false},
          Category{TokenType::TYPE, "\\btype\\b", false},

          Category{TokenType::SWITCH, "\\bswitch\\b", false},
          Category{TokenType::CASE, "\\bcase\\b", false},
          Category{TokenType::DEFAULT, "\\bdefault\\b", false},

          Category{TokenType::IF, "\\bif\\b", false},
          Category{TokenType::ELSE, "\\belse\\b", false},

          Category{TokenType::WHILE, "\\bwhile\\b", false},
          Category{TokenType::FOR, "\\bfor\\b", false},
          Category{TokenType::STOP, "\\bstop\\b", false},
          Category{TokenType::PASS, "\\bpass\\b", false},

          Category{TokenType::RETURN, "\\breturn\\b", false},
          Category{TokenType::FUNCTION, "\\bdef\\b", false},

          Category{TokenType::ENUM, "\\benum\\b", false},
          Category{TokenType::STRUCT, "\\bstruct\\b", false},
          Category{TokenType::CLASS, "\\bclass\\b", false},
          Category{TokenType::PRIVATE, "\\bprivate\\b", false},
          Category{TokenType::PUBLIC, "\\bpublic\\b", false},
          Category{TokenType::PROTECTED, "\\bprotected\\b", false},
          Category{TokenType::EXTENDS, "\\bextends\\b", false},
          Category{TokenType::METHOD, "\\bmethod\\b", false},
          Category{TokenType::ATTR, "\\battr\\b", false},
          Category{TokenType::THIS, "\\bthis\\b", false},
          Category{TokenType::OVERRIDE, "\\boverride\\b", false},
          Category{TokenType::VIRTUAL, "\\bvirtual\\b", false},

          // str here so it does not conflict with struct
          Category{TokenType::TYPE_STR, "\\bstr\\b", false},

          Category{TokenType::SUPER, "\\bsuper\\b", false},

          Category{TokenType::OR, "\\bor\\b", false},
          Category{TokenType::AND, "\\band\\b", false},
          Category{TokenType::DO, "\\bdo\\b", false},
          // avoids the conflic between do and double //
          // "(?!\\b(do)\\b)[a-zA-Z]+[a-zA-Z0-9_]*"
          Category{TokenType::ID, "[a-zA-Z]+[a-zA-Z0-9_]*", false},
          Category{TokenType::RP, "\\)", false},
          Category{TokenType::LP, "\\(", false},
          Category{TokenType::RB, "\\}", false},
          Category{TokenType::LB, "\\{", false},
          Category{TokenType::RC, "\\]", false},
          Category{TokenType::LC, "\\[", false},
          Category{TokenType::SEMICOLON, ";", false},
          Category{TokenType::COMMA, ",", false},
          Category{TokenType::TERNARY, "\\?", false},

          Category{TokenType::DOTDOT_ASSIGNMENT, ":=", false},

          Category{TokenType::DOTDOT, ":", false},
          Category{TokenType::DOT, "\\.", false},

          Category{TokenType::SELF_ADD, "\\+=", false},
          Category{TokenType::SELF_SUB, "\\-=", false},
          Category{TokenType::SELF_MULT, "\\*=", false},
          Category{TokenType::SELF_DIV, "\\/=", false},

          Category{TokenType::SMALLEREQUAL, "<=", false},
          Category{TokenType::BIGGEREQUAL, ">=", false},
          Category{TokenType::NOTEQUAL, "!=", false},
          Category{TokenType::EQUAL, "==", false},

          Category{TokenType::ASSIGNMENT, "=", false},

          Category{TokenType::INCREMENT, "\\+\\+", false},
          Category{TokenType::DECREMENT, "\\-\\-", false},
          Category{TokenType::OPERATOR_ADD, "[+]", false},
          Category{TokenType::OPERATOR_SUB, "[-]", false},
          Category{TokenType::OPERATOR_MULT, "[\\*]", false},
          Category{TokenType::OPERATOR_DIV, "[/]", false},
          Category{TokenType::OPERATOR_SMALLER, "[<]", false},
          Category{TokenType::OPERATOR_GREATER, "[>]", false},
          Category{TokenType::OPERATOR_NOT, "[!]", false},
          Category{TokenType::OPERATOR_MODULE, "[%]", false}},
      extension()};
};

} // namespace nicole