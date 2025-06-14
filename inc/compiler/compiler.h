#ifndef COMPILER_H
#define COMPILER_H

#include "../lexicalAnalysis/nicoleSintax.h"
#include "../options/optionsParser.h"
#include "../parsingAnalysis/algorithm/topDown.h"
#include "../visitors/codeGeneration/codeGeneration.h"
#include "../visitors/fillSemanticInfo/fillSemanticInfo.h"
#include "../visitors/monomorphize/monomorphize.h"
#include "../visitors/printTree/printTree.h"
#include "../visitors/typeAnalysis/typeAnalysis.h"
#include "../visitors/validateTree/validateTree.h"
#include "jitEngine.h"
#include "linker.h"
#include "optimizer.h"
#include "targetGenerator.h"
#include <expected>
#include <memory>
#include <variant>

namespace nicole {

class Compiler {
protected:
  std::shared_ptr<Sintax> sintax_;

public:
  explicit Compiler(const std::shared_ptr<Sintax> &sintax) noexcept
      : sintax_{sintax} {}

  virtual ~Compiler() = default;

  [[nodiscard]]
  auto sintax() const noexcept -> const std::shared_ptr<Sintax> & {
    return sintax_;
  }

  [[nodiscard]]
  virtual auto compile(const Options &options) const noexcept
      -> std::expected<std::monostate, Error> = 0;
};

} // namespace nicole

#endif