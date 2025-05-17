#ifndef OPTIONS_H
#define OPTIONS_H

#include <filesystem>
#include <iostream>

namespace nicole {

class Options final {
private:
  bool help_;
  bool optimize_;
  bool printTree_;
  bool printIR_;
  bool validateTree_;
  bool justInTime_;
  std::string binaryName_;
  std::filesystem::path entryFilePath_;

public:
  explicit Options(const bool help, const bool optimize, const bool printTree,
                   const bool printIR, const bool validateTree,
                   const bool justInTime, const std::string &binaryName,
                   const std::filesystem::path &entryFilePath) noexcept
      : help_{help}, optimize_{optimize}, printTree_{printTree},
        printIR_{printIR}, validateTree_{validateTree}, justInTime_{justInTime},
        binaryName_{binaryName}, entryFilePath_{entryFilePath} {}

  [[nodiscard]] bool help() const noexcept { return help_; }

  [[nodiscard]] bool optimize() const noexcept { return optimize_; }

  [[nodiscard]] bool printTree() const noexcept { return printTree_; }

  [[nodiscard]] bool printIR() const noexcept { return printIR_; }

  [[nodiscard]] bool validateTree() const noexcept { return validateTree_; }

  [[nodiscard]] bool justInTime() const noexcept { return justInTime_; }

  [[nodiscard]] const std::string &binaryName() const noexcept {
    return binaryName_;
  }

  [[nodiscard]] const std::filesystem::path &entryFilePath() const noexcept {
    return entryFilePath_;
  }

  void helper() noexcept {
    std::cout
        << "Usage \n"
           "From the parent directory, run:\n"
           "\t./nicole.sh [[options] input_file] | "
           "-t Where input_file is the main program file with the "
           ".nc extension(e.g., helloWorld.nc).\n\n"
           "Options can appear in any position except -n, "
           "which must be followed by the output file name.\n\n"
           "\t-h | --help --> Displays a brief description of how to use the "
           "compiler.\n\n"
           "\t-v | --validate --> Forces the program to follow certain "
           "validation "
           "rules (recommended).\n\n"
           "\t-o | --optimize --> Performs optimizations on the generated "
           "code.\n\n"
           "\t-n | --name output_file --> Allows specifying the name of the "
           "output "
           "file (default is a.out).\n\n"
           "\t-p | --printTree --> Prints the Abstract Syntax Tree (AST) in a "
           "directory-like structure.\n\n"
           "\t-i | --printIR --> Prints the generated Intermediate "
           "Representation "
           "(IR) code.\n\n"
           "\t-t --> Executes the tests of the compiler, also to run the tests "
           "no "
           "other argument but -t can be passed to the script.\n\n"
           "Usage Examples:\n"
           "\t- Compile a file with optimization and validation, specifying "
           "the "
           "output executable name:\n"
           "      ./nicole.sh -v -o -n program_out helloWorld.nc\n\n"
           "\t- Generate the AST and intermediate code without optimization :\n"
           "      ./nicole.sh -p -i helloWorld.nc\n";
  }
};

} // namespace nicole

#endif