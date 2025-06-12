#include "../../inc/compiler/nicole.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <memory>

namespace nicole {

auto Nicole::compile(const Options &options) const noexcept
    -> std::expected<std::monostate, Error> {
  const nicole::TopDown topDown{sintax_};

  const std::expected<std::shared_ptr<nicole::Tree>, nicole::Error> tree{
      topDown.parse(options.entryFilePath())};

  if (!tree) {
    return createError(tree.error());
  }

  std::cout << "Finished parsing\n";

  if (options.validateTree()) {
    const nicole::ValidateTree validateTree{};
    const std::expected<bool, nicole::Error> validated{
        validateTree.validate((*tree).get())};

    if (!validated) {
      return createError(validated.error());
    }
    std::cout << "Finished validate\n";
  }

  if (options.printTree()) {
    const nicole::PrintTree printTree{};
    const std::expected<std::string, nicole::Error> toStr{
        printTree.print((*tree).get())};
    if (!toStr) {
      return createError(toStr.error());
    }
    std::cout << "|--------------------------------|\n";
    std::cout << *toStr << "\n";
    std::cout << "Finished print tree\n";
    std::cout << "|--------------------------------|\n";
  }

  std::shared_ptr<nicole::TypeTable> typeTable{
      std::make_shared<nicole::TypeTable>()};
  std::shared_ptr<nicole::FunctionTable> functionTable{
      std::make_shared<nicole::FunctionTable>()};
  const nicole::FillSemanticInfo semanticFiller{functionTable, typeTable,
                                                options};
  const std::expected<std::monostate, Error> isTablesFilled{
      semanticFiller.fill((*tree).get())};
  if (!isTablesFilled) {
    return createError(isTablesFilled.error());
  }

  std::cout << "Finished semantic analysis\n";

  if (options.printTree()) {
    const nicole::PrintTree printTree{};
    const std::expected<std::string, nicole::Error> toStr{
        printTree.print((*tree).get())};
    if (!toStr) {
      return createError(toStr.error());
    }
    std::cout << "|--------------------------------|\n";
    std::cout << *toStr << "\n";
    std::cout << "Finished print tree\n";
    std::cout << "|--------------------------------|\n";
  }

  const nicole::TypeAnalysis typeAnalysis{functionTable, typeTable};
  const std::expected<std::shared_ptr<Type>, Error> analyzed{
      typeAnalysis.analyze((*tree).get())};
  if (!analyzed) {
    return createError(analyzed.error());
  }

  std::cout << "Finished type analysis\n";

  const nicole::Monomorphize monomorphizer{functionTable, typeTable};
  const std::expected<std::monostate, Error> monomorphized{
      monomorphizer.transform((*tree).get())};
  if (!monomorphized) {
    return createError(monomorphized.error());
  }

  std::cout << "Finished monomorphization\n";

  functionTable->print();
  /*
    const std::expected<std::shared_ptr<Type>, Error>
    analyzedSecondTime{typeAnalysis.analyze((*tree).get())}; if
    (!analyzedSecondTime) { return createError(analyzedSecondTime.error());
    }
  */

  const auto targetGenerator{TargetGenerator{}};
  const auto target{targetGenerator.generate(options.arch(), options.os())};

  auto context = std::make_unique<llvm::LLVMContext>();

  const nicole::CodeGeneration codeGenerator{functionTable, typeTable, options};
  auto generatedIR{codeGenerator.generate((*tree).get())};
  if (!generatedIR) {
    return createError(generatedIR.error());
  }

  auto safeModule{std::move(generatedIR.value())};

  if (options.optimize()) {
    // Construyo un optimizador fresco para O3
    nicole::Optimizer optimizer{target, llvm::OptimizationLevel::O3};
    auto optimized = optimizer.optimize(std::move(safeModule));
    if (!optimized) {
      return createError(optimized.error());
    }
    safeModule = std::move(optimized.value());
  }

  if (options.printIR()) {
    safeModule.getModuleUnlocked()->print(llvm::outs(), nullptr);
  }

  if (options.justInTime()) {
    const auto jitErr{JIT::Create()};
    if (!jitErr) {
      return createError(jitErr.error());
    }
    const auto jit{jitErr.value().get()};
    const auto addedModuled{jit->addModule(std::move(safeModule))};
    const auto running{jit->run("main")};
    if (!running) {
      return createError(running.error());
    }
    return {};
  }

  Linker linker{options, target};
  const auto linked{linker.link(std::move(safeModule))};
  if (!linked) {
    return createError(linked.error());
  }

  return {};
}

} // namespace nicole