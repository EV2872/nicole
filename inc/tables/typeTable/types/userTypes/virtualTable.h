#ifndef VIRTUAL_TABLE_H
#define VIRTUAL_TABLE_H

#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <string>
#include <vector>

namespace nicole {

class VirtualTable {
  // Punteros constantes a funciones (destructor + otros métodos virtuales)
  std::vector<llvm::Constant *> entries_;
  llvm::GlobalVariable *gv_ = nullptr;

public:
  // Añade un slot (e.g. destructor o un método virtual)
  auto addSlot(llvm::Function *fn) noexcept -> void;

  // Crea (a la primera llamada) o devuelve la vtable global en el módulo
  [[nodiscard]] auto getOrCreateGV(llvm::Module &M, const std::string &typeName,
                                   llvm::FunctionType *slotTy) noexcept
      -> llvm::GlobalVariable *;
};

} // namespace nicole

#endif