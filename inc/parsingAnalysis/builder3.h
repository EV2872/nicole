#ifndef BUILDER3_H
#define BUILDER3_H

#include "../errors.h"
#include "ast/ast.h"
#include "ast/literals/ast_char.h"
#include "ast/literals/ast_string.h"
#include <expected>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace nicole {

class Builder3 {
private:
  [[nodiscard]] static auto generateNextId() noexcept -> unsigned long long {
    static unsigned long long id = 0;
    return ++id;
  }

    template <typename Parent, typename Child>
    requires std::is_base_of_v<AST, Child>
  static void assignFather(const std::shared_ptr<Child> &child,
                           const std::shared_ptr<Parent> &parent) noexcept {
    if (child)
      child->setFather(parent);
  }

  // 2) Especialización para vector<AST>
  template <typename Parent>
  static void assignFather(const std::vector<std::shared_ptr<AST>> &children,
                           const std::shared_ptr<Parent> &parent) noexcept {
    for (auto &c : children)
      if (c)
        c->setFather(parent);
  }

  // 3) Caída para otros tipos (no AST ni vector<AST>)
  template <typename Parent, typename T>
  static void assignFather(const T &,
                           const std::shared_ptr<Parent> &) noexcept {}

public:
  // ---------------------------------
  // 1) Especialización para AST_CHAR
  // ---------------------------------
  static auto create(const SourceLocation &srcLoc,
                     const std::string &value) noexcept
      -> std::expected<std::shared_ptr<AST_CHAR>, Error> {
    // Quitar las comillas
    if (value.size() < 2) {
      return createError(ERROR_TYPE::SINTAX, "mal formed char literal");
    }
    std::string body = value.substr(1, value.size() - 2);
    char result = '\0';

    if (body.size() == 1) {
      result = body[0];
    } else if (body.size() == 2 && body[0] == '\\') {
      switch (body[1]) {
      case 'n':
        result = '\n';
        break;
      case 't':
        result = '\t';
        break;
      case '\\':
        result = '\\';
        break;
      case '\'':
        result = '\'';
        break;
      case '\"':
        result = '\"';
        break;
      default:
        result = body[1];
        break;
      }
    } else {
      return createError(ERROR_TYPE::SINTAX, "invalid char escape");
    }

    auto node = std::make_shared<AST_CHAR>(generateNextId(), srcLoc, result);
    return node;
  }

  // -----------------------------------
  // 2) Especialización para AST_STRING
  // -----------------------------------
  static auto createString(const SourceLocation &srcLoc,
                           const std::string &value) noexcept
      -> std::expected<std::shared_ptr<AST_STRING>, Error> {
    if (value.size() < 2) {
      return createError(ERROR_TYPE::SINTAX, "mal formed string literal");
    }
    std::string body = value.substr(1, value.size() - 2);
    std::string result;
    result.reserve(body.size());

    for (size_t i = 0; i < body.size(); ++i) {
      if (body[i] == '\\' && i + 1 < body.size()) {
        switch (body[i + 1]) {
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
          result.push_back(body[i + 1]);
          break;
        }
        ++i; // saltar el carácter escapado
      } else {
        result.push_back(body[i]);
      }
    }

    auto node = std::make_shared<AST_STRING>(generateNextId(), srcLoc, result);
    return node;
  }

  // --------------------------------------------
  // 3) Plantilla genérica para todos los demás
  // --------------------------------------------
  template <typename Node, typename... Args>
    requires std::is_base_of_v<AST, Node> &&
             (!std::is_same_v<Node, AST_CHAR>) &&
             (!std::is_same_v<Node, AST_STRING>)
  static auto create(const SourceLocation &srcLoc, Args &&...args) noexcept
      -> std::expected<std::shared_ptr<Node>, Error> {
    auto node = std::make_shared<Node>(generateNextId(), srcLoc,
                                       std::forward<Args>(args)...);

    // Asignar padre a cada argumento que sea AST o vector<AST>
    (assignFather(std::forward<Args>(args), node), ...);

    return node;
  }
};

} // namespace nicole

#endif // BUILDER2_H
