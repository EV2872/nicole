#include "../../../inc/visitors/codeGeneration/codeGeneration.h"

#include "../../../inc/parsingAnalysis/ast/conditionals/ast_case.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_default.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_elseIf.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_if.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_switch.h"
#include "../../../inc/parsingAnalysis/ast/conditionals/ast_ternary.h"
#include <cstdint>

namespace nicole {

auto CodeGeneration::visit(const AST_IF *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_IF");
  }
  std::expected<llvm::Value *, Error> condOrErr =
      emitRValue(node->condition().get());
  if (!condOrErr)
    return createError(condOrErr.error());
  llvm::Value *condVal = *condOrErr; // debe ser i1

  // preparar bloques
  llvm::Function *parent = builder_.GetInsertBlock()->getParent();
  std::string id = std::to_string(node->nodeId());
  llvm::BasicBlock *thenBB =
      llvm::BasicBlock::Create(*context_, "if_then" + id, parent);
  llvm::BasicBlock *elseBB =
      llvm::BasicBlock::Create(*context_, "if_else" + id, parent);
  llvm::BasicBlock *mergeBB =
      llvm::BasicBlock::Create(*context_, "if_merge" + id, parent);
  currentMergeBlock_ = mergeBB;

  // salto condicional
  builder_.CreateCondBr(condVal, thenBB, elseBB);

  // THEN block
  builder_.SetInsertPoint(thenBB);
  if (std::expected<llvm::Value *, Error> thenOrErr =
          node->body()->accept(*this);
      !thenOrErr)
    return createError(thenOrErr.error());
  if (!builder_.GetInsertBlock()->getTerminator())
    builder_.CreateBr(mergeBB);

  // ELSE / ELSE IF chain
  builder_.SetInsertPoint(elseBB);
  if (!node->elseIf().empty() || node->elseBody()) {
    // anidar else-if
    for (const std::shared_ptr<AST_ELSE_IF> &elif : node->elseIf()) {
      // cada AST_ELSE_IF visitor generará su propio codegen,
      // creando bloques nuevos y saltos a mergeBB o al siguiente else
      if (std::expected<llvm::Value *, Error> elifOrErr = elif->accept(*this);
          !elifOrErr)
        return createError(elifOrErr.error());
      // al salir de cada elif, el builder_ estará apuntando al bloque
      // siguiente (o al merge) listo para el próximo elif / else
    }
    // else final
    if (node->elseBody()) {
      if (std::expected<llvm::Value *, Error> elseOrErr =
              node->elseBody()->accept(*this);
          !elseOrErr)
        return createError(elseOrErr.error());
      if (!builder_.GetInsertBlock()->getTerminator())
        builder_.CreateBr(mergeBB);
    }
    // Si había else-if pero NO hay else, asegurar br a merge
    if (node->elseIf().size() > 0 && !node->elseBody()) {
      if (!builder_.GetInsertBlock()->getTerminator())
        builder_.CreateBr(mergeBB);
    }
  } else {
    // no hay else, saltar directamente a merge
    if (!builder_.GetInsertBlock()->getTerminator())
      builder_.CreateBr(mergeBB);
  }

  // merge
  builder_.SetInsertPoint(mergeBB);
  return nullptr; // el if no produce un value útil por sí solo
}

auto CodeGeneration::visit(const AST_ELSE_IF *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_ELSE_IF");
  std::expected<llvm::Value *, Error> condOrErr =
      emitRValue(node->condition().get());
  if (!condOrErr)
    return createError(condOrErr.error());
  llvm::Value *condVal = *condOrErr;

  // Crear bloques for this else-if
  llvm::Function *parent = builder_.GetInsertBlock()->getParent();
  std::string id = std::to_string(node->nodeId());
  llvm::BasicBlock *thenBB =
      llvm::BasicBlock::Create(*context_, "elif_then" + id, parent);
  llvm::BasicBlock *nextBB =
      llvm::BasicBlock::Create(*context_, "elif_next" + id, parent);

  // CondBr al then o al siguiente (que puede ser otro elif o merge)
  llvm::BasicBlock *mergeBB = currentMergeBlock_;
  builder_.CreateCondBr(condVal, thenBB, nextBB);

  // THEN de este elif
  builder_.SetInsertPoint(thenBB);
  if (std::expected<llvm::Value *, Error> r = node->body()->accept(*this); !r)
    return createError(r.error());
  if (!builder_.GetInsertBlock()->getTerminator())
    builder_.CreateBr(mergeBB);

  // Continuar en el siguiente bloque (nextBB)
  builder_.SetInsertPoint(nextBB);
  return nullptr;
}

auto CodeGeneration::visit(const AST_SWITCH *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_SWITCH");

  // Evaluar la condición
  std::expected<llvm::Value *, Error> condOrErr =
      emitRValue(node->condition().get());
  if (!condOrErr)
    return createError(condOrErr.error());
  llvm::Value *condVal = *condOrErr;

  // Preparar bloques y switchInst
  llvm::Function *parent = builder_.GetInsertBlock()->getParent();
  std::string id = std::to_string(node->nodeId());

  // Bloque final (merge)
  llvm::BasicBlock *mergeBB =
      llvm::BasicBlock::Create(*context_, "switch_merge" + id, parent);

  // Empujar destino de break para 'stop' dentro del switch
  breakTargets_.push(mergeBB);

  // Bloque por defecto (o salto directo a merge si no hay default)
  llvm::BasicBlock *defaultBB =
      node->defaultCase()
          ? llvm::BasicBlock::Create(*context_, "switch_default" + id, parent)
          : mergeBB;

  // Crear la instrucción switch
  llvm::SwitchInst *sw = builder_.CreateSwitch(
      condVal, defaultBB, static_cast<uint32_t>(node->cases().size()));

  // Recorrer cada case: crear bloque, registrarlo y emitir body
  for (const std::shared_ptr<AST_CASE> &caseNode : node->cases()) {
    // Evaluar el literal del case
    std::expected<llvm::Value *, Error> litOrErr =
        emitRValue(caseNode->match().get());
    if (!litOrErr)
      return createError(litOrErr.error());
    llvm::ConstantInt *litVal = llvm::dyn_cast<llvm::ConstantInt>(*litOrErr);
    if (!litVal)
      return createError(ERROR_TYPE::TYPE,
                         "switch case match is not a constant integer");

    // Crear el BasicBlock para este case
    std::string caseNum = std::to_string(litVal->getZExtValue());
    llvm::BasicBlock *caseBB = llvm::BasicBlock::Create(
        *context_, "switch_case" + id + "_" + caseNum, parent);

    // Registrar el case en el switch
    sw->addCase(litVal, caseBB);

    // Emitir el cuerpo del case
    builder_.SetInsertPoint(caseBB);
    if (std::expected<llvm::Value *, Error> r = caseNode->body()->accept(*this);
        !r)
      return createError(r.error());
    if (!builder_.GetInsertBlock()->getTerminator())
      builder_.CreateBr(mergeBB);
  }

  // Emitir default si existe
  if (node->defaultCase()) {
    builder_.SetInsertPoint(defaultBB);
    if (std::expected<llvm::Value *, Error> r =
            node->defaultCase()->body()->accept(*this);
        !r)
      return createError(r.error());
    if (!builder_.GetInsertBlock()->getTerminator())
      builder_.CreateBr(mergeBB);
  }

  // Popear la pila de breaks al salir del switch
  breakTargets_.pop();

  // Continuar en merge
  builder_.SetInsertPoint(mergeBB);
  return nullptr;
}

auto CodeGeneration::visit(const AST_CASE *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  // Ya manejado en visit(AST_SWITCH*); aquí solo por completitud
  return node->body()->accept(*this);
}

auto CodeGeneration::visit(const AST_DEFAULT *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  // Similarmente, su cuerpo ya se generó en visit(AST_SWITCH*)
  return node->body()->accept(*this);
}

auto CodeGeneration::visit(const AST_TERNARY *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  if (!node)
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_TERNARY");

  // condición como rvalue (i1)
  std::expected<llvm::Value *, Error> condOrErr =
      emitRValue(node->condition().get());
  if (!condOrErr)
    return createError(condOrErr.error());
  llvm::Value *condVal = *condOrErr;

  // bloques
  llvm::Function *parent = builder_.GetInsertBlock()->getParent();
  std::string id = std::to_string(node->nodeId());
  llvm::BasicBlock *thenBB =
      llvm::BasicBlock::Create(*context_, "tern_then" + id, parent);
  llvm::BasicBlock *elseBB =
      llvm::BasicBlock::Create(*context_, "tern_else" + id, parent);
  llvm::BasicBlock *mergeBB =
      llvm::BasicBlock::Create(*context_, "tern_merge" + id, parent);

  builder_.CreateCondBr(condVal, thenBB, elseBB);

  // THEN branch: rvalue de la primera expresión
  builder_.SetInsertPoint(thenBB);
  std::expected<llvm::Value *, Error> thenOrErr =
      emitRValue(node->first().get());
  if (!thenOrErr)
    return createError(thenOrErr.error());
  llvm::Value *thenVal = *thenOrErr; // ¡aquí es un i32, no un ptr!
  builder_.CreateBr(mergeBB);

  // ELSE branch: rvalue de la segunda expresión
  builder_.SetInsertPoint(elseBB);
  std::expected<llvm::Value *, Error> elseOrErr =
      emitRValue(node->second().get());
  if (!elseOrErr)
    return createError(elseOrErr.error());
  llvm::Value *elseVal = *elseOrErr; // también un i32
  builder_.CreateBr(mergeBB);

  // Merge y PHI: ahora both son i32
  builder_.SetInsertPoint(mergeBB);
  llvm::PHINode *phi =
      builder_.CreatePHI(thenVal->getType(), 2, "tern_phi" + id);
  phi->addIncoming(thenVal, thenBB);
  phi->addIncoming(elseVal, elseBB);

  return phi;
}

auto CodeGeneration::visit(const AST_CONDITION *node) const noexcept
    -> std::expected<llvm::Value *, Error> {
  if (!node) {
    return createError(ERROR_TYPE::NULL_NODE, "invalid AST_CONDITION");
  }
  return node->condition()->accept(*this);
}

} // namespace nicole