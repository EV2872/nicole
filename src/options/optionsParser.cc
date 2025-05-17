#include "../../inc/options/optionsParser.h"

namespace nicole {

bool OptionsParser::isOption(const std::string_view argument) noexcept {
  if (argument == "-o" or argument == "--optimize" or argument == "-n" or
      argument == "--name" or argument == "-h" or argument == "--help" or
      argument == "-p" or argument == "--printTree" or argument == "-i" or
      argument == "--printIR" or argument == "-v" or argument == "--validate" or
      argument == "-j" or argument == "--justInTime") {
    return true;
  }
  return false;
}

const std::expected<Options, Error>
OptionsParser::parse(const std::vector<std::string_view> &arguments) noexcept {
  const std::size_t size{arguments.size()};

  if (!size) {
    return createError(ERROR_TYPE::MISSING_ARGUMENTS,
                       "at least must give the name of the entry file");
  }

  bool help{false};
  bool optimize{false};
  bool printTree{false};
  bool printIR{false};
  bool validateTree{false};
  bool justInTime{false};
  std::string binaryName{"a.out"};
  std::filesystem::path entryFilePath{""};

  bool entryFileParsed{false};

  for (std::size_t i{0}; i < size; ++i) {
    const std::string_view argument{arguments[i]};
    if (argument == "-h" or argument == "--help") {
      help = true;
      // break; not using break in case that we let the user use help and
      // execute
    } else if (argument == "-o" or argument == "--optimize") {
      optimize = true;
    } else if (argument == "-p" or argument == "--printTree") {
      printTree = true;
    } else if (argument == "-i" or argument == "--printIR") {
      printIR = true;
    } else if (argument == "-v" or argument == "--validate") {
      validateTree = true;
    } else if (argument == "-j" or argument == "--justInTime") {
      justInTime = true;
    } else if (argument == "-n" or argument == "--name") {
      if (++i == size or isOption(arguments[i])) {
        return createError(ERROR_TYPE::MISSING_BINARY_NAME,
                           "missing binary name");
      }
      binaryName = arguments[i];
    } else {
      if (entryFileParsed) {
        return createError(ERROR_TYPE::UNKOWN_ARGUMENT,
                           "argument: " + std::string{argument} +
                               " not supported");
      }
      entryFilePath = argument;
      entryFileParsed = true;
    }
  }
  // in case that help exists we skip the error
  if (entryFilePath.empty() and !help) {
    return createError(ERROR_TYPE::MISSING_ENTRY_FILE,
                       "must specify the entry file");
  }

  return Options{help,         optimize,   printTree,  printIR,
                 validateTree, justInTime, binaryName, entryFilePath};
}

} // namespace nicole