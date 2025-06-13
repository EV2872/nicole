#include "../../inc/utils/nameMangler.h"
#include <memory>

namespace nicole {

auto NameMangler::nameMangling(const std::shared_ptr<Type> &type) noexcept
    -> std::expected<std::string, Error> {
  std::string mangled{};
  auto res = nameManglingImpl(type, mangled);
  if (!res)
    return res;
  // quitar posible guión bajo inicial
  if (!mangled.empty() && mangled.front() == '_')
    mangled.erase(mangled.begin());
  return mangled;
}

auto NameMangler::nameManglingImpl(const std::shared_ptr<Type> &type,
                                   std::string &result) noexcept
    -> std::expected<std::string, Error> {
  if (!type)
    return createError(ERROR_TYPE::TYPE, "null type in name mangling");

  if (auto bt = std::dynamic_pointer_cast<BasicType>(type)) {
    result += '_' + bt->toString();
    return result;
  }
  if (std::dynamic_pointer_cast<VoidType>(type)) {
    result += "_void";
    return result;
  }
  if (std::dynamic_pointer_cast<NullType>(type)) {
    result += "_null";
    return result;
  }
  if (auto pt = std::dynamic_pointer_cast<PointerType>(type)) {
    auto rec = nameManglingImpl(pt->baseType(), result);
    if (!rec)
      return rec;
    result += "_ptr";
    return result;
  }
  if (auto vt = std::dynamic_pointer_cast<VectorType>(type)) {
    auto rec = nameManglingImpl(vt->elementType(), result);
    if (!rec)
      return rec;
    result += "_vec";
    return result;
  }
  if (auto ct = std::dynamic_pointer_cast<ConstType>(type)) {
    result += "_const";
    auto rec = nameManglingImpl(ct->baseType(), result);
    if (!rec)
      return rec;
    return result;
  }
  if (auto et = std::dynamic_pointer_cast<EnumType>(type)) {
    result += '_' + et->name();
    return result;
  }
  if (auto git = std::dynamic_pointer_cast<GenericInstanceType>(type)) {
    result += '_' + git->name();
    for (auto &arg : git->typeArgs()) {
      auto rec = nameManglingImpl(arg, result);
      if (!rec)
        return rec;
    }
    return result;
  }
  if (auto ut = std::dynamic_pointer_cast<UserType>(type)) {
    result += '_' + ut->name();
    return result;
  }
  // Fallback
  std::string raw = type->toString();
  for (char c : raw) {
    result += (std::isalnum(static_cast<unsigned char>(c)) ? c : '_');
  }
  return result;
}

auto NameMangler::mangleFunction(
    const std::shared_ptr<AST_FUNC_DECL> &node,
    const std::vector<std::shared_ptr<Type>> &replacements) noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE,
                       "the node at function mangling is null");
  }
  if (node->generics().size()) {
    return createError(ERROR_TYPE::FUNCTION,
                       "cannot mangle functions with generics");
  }
  std::string mangled{prefix_ + node->id()};
  for (const auto &replacement : replacements) {
    auto res = nameMangling(replacement);
    if (!res)
      return res;
    mangled += *res;
  }
  for (const auto &parameter : node->parameters()) {
    auto res = nameMangling(parameter.second);
    if (!res)
      return res;
    mangled += *res;
  }
  mangled += "_ret";
  auto res = nameMangling(node->returnType());
  if (!res)
    return res;
  mangled += *res;
  return mangled;
}

auto NameMangler::mangleClass(
    const std::shared_ptr<AST_STRUCT> &node,
    const std::vector<std::shared_ptr<Type>> &replacements) noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE,
                       "the node at struct mangling is null");
  }
  if (node->generics().size()) {
    return createError(ERROR_TYPE::FUNCTION,
                       "cannot mangle structs with generics");
  }
  std::string mangled{prefix_ + node->id()};
  for (const auto &replacement : replacements) {
    auto res = nameMangling(replacement);
    if (!res)
      return res;
    mangled += *res;
  }
  return mangled;
}

auto NameMangler::mangleConstructor(
    const std::shared_ptr<AST_CONSTRUCTOR_DECL> &node,
    const std::string &constructorOwner,
    const std::vector<std::shared_ptr<Type>> &replacements) noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE,
                       "the node at constructor mangling is null");
  }
  if (node->generics().size()) {
    return createError(ERROR_TYPE::FUNCTION,
                       "cannot mangle destructors with generics");
  }
  std::string mangled{prefix_ + constructorOwner + "_ctor"};
  for (const auto &replacement : replacements) {
    auto res = nameMangling(replacement);
    if (!res)
      return res;
    mangled += *res;
  }
  return mangled;
}

auto NameMangler::mangleDestructor(
    const std::shared_ptr<AST_DESTRUCTOR_DECL> &node,
    const std::string &desstructorOwner) noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE,
                       "the node at destructor mangling is null");
  }
  return prefix_ + desstructorOwner + "_dtor";
}

auto NameMangler::mangleMethod(
    const std::shared_ptr<AST_METHOD_DECL> &node,
    const std::string &methodOwner,
    const std::vector<std::shared_ptr<Type>> &replacements) noexcept
    -> std::expected<std::string, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE,
                       "the node at method mangling is null");
  }
  if (node->generics().size()) {
    return createError(ERROR_TYPE::FUNCTION,
                       "cannot mangle methods with generics");
  }
  std::string mangled{prefix_ + methodOwner + node->id()};
  for (const auto &replacement : replacements) {
    auto res = nameMangling(replacement);
    if (!res)
      return res;
    mangled += *res;
  }
  for (const auto &parameter : node->parameters()) {
    auto res = nameMangling(parameter.second);
    if (!res)
      return res;
    mangled += *res;
  }
  mangled += "_ret";
  auto res = nameMangling(node->returnType());
  if (!res)
    return res;
  mangled += *res;
  return mangled;
}

} // namespace nicole