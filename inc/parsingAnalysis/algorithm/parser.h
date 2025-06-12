#ifndef PARSER_H
#define PARSER_H

#include "../../lexicalAnalysis/sintax.h"
#include "../ast/tree.h"
#include <set>
#include <vector>

namespace nicole {

class Parser {
protected:
  Lexer lexer_;
  mutable TokenStream tkStream_{{}};
  mutable std::set<std::filesystem::path> parsedFiles_{};

public:
  explicit Parser(const std::shared_ptr<Sintax> &sintax) noexcept
      : lexer_{sintax->createLexer()} {}

  virtual ~Parser() noexcept = default;

  [[nodiscard]] virtual auto
  parse(const std::filesystem::path &entryFile) const noexcept
      -> std::expected<std::shared_ptr<Tree>, Error> = 0;
};

} // namespace nicole

#endif // AST_H
