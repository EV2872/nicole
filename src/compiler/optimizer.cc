#include "../../inc/compiler/optimizer.h"
#include <llvm/IR/Verifier.h>

namespace nicole {

auto Optimizer::optimize(llvm::orc::ThreadSafeModule TSM) noexcept
    -> std::expected<llvm::orc::ThreadSafeModule, Error> {
  llvm::Module *module{TSM.getModuleUnlocked()};
  // Si tenemos target y layout por defecto, lo asignamos:
  if (targetMachine_ && module->getDataLayout().isDefault()) {
    module->setDataLayout(targetMachine_->createDataLayout());
  }

  // Managers locales, recreados en cada llamada:
  llvm::LoopAnalysisManager LAM;
  llvm::FunctionAnalysisManager FAM;
  llvm::CGSCCAnalysisManager CGAM;
  llvm::ModuleAnalysisManager MAM;

  llvm::PassBuilder PB{targetMachine_.get()};
  PB.registerModuleAnalyses(MAM);
  PB.registerCGSCCAnalyses(CGAM);
  PB.registerFunctionAnalyses(FAM);
  PB.registerLoopAnalyses(LAM);
  PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

  // Verificación antes
  if (llvm::verifyModule(*module, &llvm::errs())) {
    return createError(ERROR_TYPE::TYPE,
                       "module verification failed before optimization");
  }

  // Pipeline por módulo
  auto MPM = PB.buildPerModuleDefaultPipeline(optLevel_, /*verifyEach=*/true);
  MPM.run(*module, MAM);

  // Verificación después
  if (llvm::verifyModule(*module, &llvm::errs())) {
    return createError(ERROR_TYPE::TYPE,
                       "module verification failed after optimization");
  }

  return std::move(TSM); // RVO se encarga de mover correctamente
}

} // namespace nicole
