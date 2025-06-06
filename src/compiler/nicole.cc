#include "../../inc/compiler/nicole.h"
#include <memory>

namespace nicole {

std::expected<std::monostate, Error>
Nicole::compile(const Options &options) const noexcept {
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
  const nicole::CodeGeneration codeGenerator{functionTable, typeTable, options};
  const std::expected<llvm::Value *, Error> generatedIR{
      codeGenerator.generate((*tree).get())};
  if (!generatedIR) {
    return createError(generatedIR.error());
  }

  return {};
}

} // namespace nicole