#include "../../../inc/parsingAnalysis/ast/functions/ast_funcCall.h"
#include "../../../inc/parsingAnalysis/ast/functions/ast_funcDecl.h"
#include "../../../inc/parsingAnalysis/ast/functions/ast_return.h"
#include "../../../inc/visitors/monomorphize/monomorphize.h"
#include <cstddef>
#include <memory>
#include <variant>

namespace nicole {

/*
Needs to be monomporhized
- Comprobar que tenga genericos
- si no hay genericos realizar copia

def foo<T>() {
  foo2<T>();
}

*/
auto Monomorphize::visit(const AST_FUNC_CALL *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "Invalid AST_FUNC_CALL");
  for (const auto &arg : node->parameters()) {
    const auto result{arg->accept(*this)};
    if (!result) {
      return createError(result.error());
    }
  }
  if (!node->replaceOfGenerics().size()) {
    return {};
  }

  // Si la llamada se realiza dentro de otra funcion con genericos
  if (currentCallReplacements_.size()) {
    // si algun substituto de un generico sigue siendo generico debemos esperar:
    // ejemplo: def foo<T>(): T { foo2<T>(); ...}
    for (const auto &replacement : node->replaceOfGenerics()) {
      if (typeTable_->isCompundPlaceHolder(replacement)) {
        return {};
      }
    }
    for (const auto &arg : node->parameters()) {
      if (typeTable_->isCompundPlaceHolder(arg->returnedFromTypeAnalysis())) {
        return {};
      }
    }
  }
  if (!currentCallReplacements_.size()) {
    // si no esta dentro de otra declaracion
    currentCallReplacements_ = node->replaceOfGenerics();
  } else {
    // Si esta dentro de otra declaracion con genericos debemos tener encuenta
    // los substitutos padres
    auto auxiliar{node->replaceOfGenerics()};
    for (auto &auxRpl : auxiliar) {
      if (typeTable_->isCompundPlaceHolder(auxRpl)) {
        // currentGenericList de la declaracion
        const auto substitute{typeTable_->applyGenericReplacements(
            auxRpl, currentGenericList_, currentCallReplacements_)};
        if (!substitute) {
          return createError(substitute.error());
        }
        auxRpl = *substitute;
      }
    }
    currentCallReplacements_ = auxiliar;
  }

  // resolver sobrecarga de funciones
  std::vector<std::shared_ptr<Type>> argTypes;
  for (const auto &expr : node->parameters())
    argTypes.push_back(expr->returnedFromTypeAnalysis());
  auto &declList = funcDeclReferences.at(node->id());
  std::vector<std::shared_ptr<AST_FUNC_DECL>> viableDecls;
  const auto &explicitGens = node->replaceOfGenerics();

  for (auto &decl : declList) {
    auto params = decl->parameters().params();
    if (params.size() != argTypes.size())
      continue;
    if (!explicitGens.empty() && decl->generics().size() != explicitGens.size())
      continue;

    bool matches = true;
    bool hasExplicit = !explicitGens.empty();
    for (size_t i = 0; i < params.size(); ++i) {
      auto pty = params[i].second;
      auto aty = argTypes[i];
      if ((typeTable_->isCompundPlaceHolder(pty) ||
           typeTable_->isCompundPlaceHolder(aty)) &&
          !hasExplicit) {
        // aún no resuelto → posponer
        matches = false;
        break;
      }
      if (!typeTable_->canAssign(pty, aty)) {
        matches = false;
        break;
      }
    }
    if (matches)
      viableDecls.push_back(decl);
  }

  if (viableDecls.empty())
    return createError(ERROR_TYPE::FUNCTION,
                       "no matching template instantiation for: " + node->id());
  if (viableDecls.size() > 1)
    return createError(ERROR_TYPE::FUNCTION,
                       "ambiguous template instantiation for: " + node->id());

  auto funcOriginalDecl = viableDecls.front();
  // en vez de currentGenericLists para no encontrar basura
  auto declGenerics = funcOriginalDecl->generics();
  // realizar copia
  auto copyFunDecl{std::make_shared<AST_FUNC_DECL>(*funcOriginalDecl)};

  // substituir parametros y tipo de retorno
  std::vector<std::pair<std::string, std::shared_ptr<Type>>> newParams{};
  std::expected<std::shared_ptr<Type>, Error> substituteParam{nullptr};
  for (auto param : copyFunDecl->parameters()) {
    if (typeTable_->isCompundPlaceHolder(param.second)) {
      substituteParam = typeTable_->applyGenericReplacements(
          param.second, declGenerics, currentCallReplacements_);
      if (!substituteParam) {
        return createError(substituteParam.error());
      }
      param.second = *substituteParam;
    }
    newParams.push_back({param.first, param.second});
  }

  std::expected<std::shared_ptr<Type>, Error> substituteReturnType{nullptr};
  if (typeTable_->isCompundPlaceHolder(copyFunDecl->returnType())) {
    substituteReturnType = typeTable_->applyGenericReplacements(
        copyFunDecl->returnType(), declGenerics, currentCallReplacements_);
    if (!substituteReturnType) {
      return createError(substituteReturnType.error());
    }
    copyFunDecl->setReturnType(*substituteReturnType);
  }

  copyFunDecl->setParameters(Parameters{newParams});

  Function mono{copyFunDecl->id(),
                {},
                copyFunDecl->parameters(),
                copyFunDecl->returnType(),
                copyFunDecl->body()};
  auto mname = nameManglingFunction(mono, currentCallReplacements_);
  if (!mname)
    return createError(mname.error());
  mono = Function{*mname,
                  {},
                  copyFunDecl->parameters(),
                  copyFunDecl->returnType(),
                  copyFunDecl->body()};
  // si previamente no fue monomorfizado
  if (!specializedFunctions_.count(*mname)) {
    functionTable_->insert(mono);
    specializedFunctions_.insert(*mname);
    // visitamos su declaracion en busqueda de mas nodos func_call
    const auto copyy{copyFunDecl->accept(*this)};
    if (!copyy) {
      return createError(copyy.error());
    }
    copyFunDecl->setId(*mname);
    copyFunDecl->setGenerics({});
    node->setDeclReference(copyFunDecl.get());
  }
  currentCallReplacements_.clear();
  return {};
}

auto Monomorphize::visit(const AST_FUNC_DECL *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_FUNC_DECL");
  if (!node->generics().size()) {
    return {};
  }
  currentGenericList_ = node->generics();
  insideDeclWithGenerics = true;
  bool alreadyStoredRef{false};
  if (funcDeclReferences.count(node->id())) {
    const auto vec{funcDeclReferences.at(node->id())};
    for (const auto &decl : vec) {
      if (decl->nodeId() == node->nodeId()) {
        alreadyStoredRef = true;
      }
    }
  }
  if (!alreadyStoredRef) {
    funcDeclReferences[node->id()].push_back(
        std::make_shared<AST_FUNC_DECL>(*node));
  }
  const auto body{node->body()->accept(*this)};
  if (!body) {
    return createError(body.error());
  }
  currentCallReplacements_.clear();
  currentGenericList_.clear();
  insideDeclWithGenerics = false;
  return {};
}

auto Monomorphize::visit(const AST_RETURN *node) const noexcept
    -> std::expected<std::monostate, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_RETURN");
  }
  if (!node->expression()) {
    return {};
  }
  const auto result{node->expression()->accept(*this)};
  if (!result) {
    return createError(result.error());
  }
  return {};
}

} // namespace nicole