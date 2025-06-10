#ifndef OPTIONS_PARSER_H
#define OPTIONS_PARSER_H

#include "../errors.h" // createError, ERROR_TYPE
#include "options.h"
#include <expected>
#include <llvm/Support/CommandLine.h>

namespace nicole {

class OptionsParser {
public:
  // parsea argc/argv con LLVM CommandLine y devuelve Options o Error
  static std::expected<Options, Error> parse(int argc, char **argv) noexcept;
};

} // namespace nicole

#endif // OPTIONS_PARSER_H
