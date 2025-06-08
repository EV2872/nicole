#include "../../../inc/tables/typeTable/types/userTypes/virtualTable.h"
#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>

namespace nicole {

void VirtualTable::addSlot(llvm::Function *fn) {
  // Todas las funciones deben ser constantes conocidas en tiempo de compilación
  entries_.push_back(llvm::ConstantExpr::getBitCast(fn, fn->getType()));
}

llvm::GlobalVariable *VirtualTable::getOrCreateGV(llvm::Module &M,
                                                  const std::string &typeName,
                                                  llvm::FunctionType *slotTy) {
  if (gv_)
    return gv_;

  // Tipo del array: N punteros a función
  auto *ptrTy = llvm::PointerType::getUnqual(slotTy);
  auto *arrTy = llvm::ArrayType::get(ptrTy, entries_.size());

  // Inicializador: array de punteros
  llvm::Constant *init = llvm::ConstantArray::get(arrTy, entries_);

  // Crear global readonly
  gv_ = new llvm::GlobalVariable(M, arrTy,
                                 /*isConstant=*/true,
                                 llvm::GlobalValue::ExternalLinkage, init,
                                 typeName + "_vtable");
  gv_->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
  gv_->setAlignment(llvm::Align(8));
  return gv_;
}

} // namespace nicole
