#include "../../inc/utils/nameMangler.h"

namespace nicole {

std::expected<std::string, Error> NameMangler::mangleFunction(
    const std::shared_ptr<AST_FUNC_DECL> &node) noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE,
                       "the node at function mangling is null");
  }
  if (node->generics().size()) {
    return createError(ERROR_TYPE::FUNCTION,
                       "cannot mangle functions with generics");
  }
  std::string result{prefix_};
  return result;
}

std::expected<std::string, Error>
NameMangler::mangleClass(const std::shared_ptr<AST_STRUCT> &node) noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE,
                       "the node at struct mangling is null");
  }
  if (node->generics().size()) {
    return createError(ERROR_TYPE::FUNCTION,
                       "cannot mangle structs with generics");
  }
  std::string result{prefix_};
  return result;
}

std::expected<std::string, Error> NameMangler::mangleConstructor(
    const std::shared_ptr<AST_CONSTRUCTOR_DECL> &node,
    const std::string &constructorOwner) noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE,
                       "the node at constructor mangling is null");
  }
  if (node->generics().size()) {
    return createError(ERROR_TYPE::FUNCTION,
                       "cannot mangle destructors with generics");
  }
  std::string result{prefix_ + constructorOwner};
  return result;
}

std::expected<std::string, Error>
NameMangler::mangleDestructor(const std::shared_ptr<AST_DESTRUCTOR_DECL> &node,
                              const std::string &desstructorOwner) noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE,
                       "the node at destructor mangling is null");
  }
  std::string result{prefix_ + desstructorOwner};
  return result;
}

std::expected<std::string, Error>
NameMangler::mangleMethod(const std::shared_ptr<AST_METHOD_DECL> &node,
                          const std::string &methodOwner) noexcept {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE,
                       "the node at method mangling is null");
  }
  if (node->generics().size()) {
    return createError(ERROR_TYPE::FUNCTION,
                       "cannot mangle methods with generics");
  }
  std::string result{prefix_ + methodOwner};
  return result;
}

} // namespace nicole