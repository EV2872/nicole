#ifndef OPTIONS_H
#define OPTIONS_H

#include <filesystem>
#include <iostream>

namespace nicole {

// Arquitecturas soportadas
enum class Arch { x86, x86_64, arm, aarch64 };

// Sistemas operativos soportados
enum class OS { Linux, MacOS, Windows };

class Options final {
private:
  bool optimize_;
  bool printTree_;
  bool printIR_;
  bool validateTree_;
  bool justInTime_;
  bool useLLD_;
  std::string binaryName_;
  std::filesystem::path entryFilePath_;
  Arch arch_;
  OS os_;

public:
  explicit Options(bool optimize, bool printTree, bool printIR,
                   bool validateTree, bool justInTime, bool useLLD,
                   const std::string &binaryName,
                   const std::filesystem::path &entryFilePath, Arch arch,
                   OS os) noexcept
      : optimize_{optimize}, printTree_{printTree}, printIR_{printIR},
        validateTree_{validateTree}, justInTime_{justInTime}, useLLD_{useLLD},
        binaryName_{binaryName}, entryFilePath_{entryFilePath}, arch_{arch},
        os_{os} {}

  [[nodiscard]] bool optimize() const noexcept { return optimize_; }

  [[nodiscard]] bool printTree() const noexcept { return printTree_; }

  [[nodiscard]] bool printIR() const noexcept { return printIR_; }

  [[nodiscard]] bool validateTree() const noexcept { return validateTree_; }

  [[nodiscard]] bool justInTime() const noexcept { return justInTime_; }

  [[nodiscard]] bool useLLD() const noexcept { return useLLD_; }

  [[nodiscard]] const std::string &binaryName() const noexcept {
    return binaryName_;
  }

  [[nodiscard]] const std::filesystem::path &entryFilePath() const noexcept {
    return entryFilePath_;
  }

  [[nodiscard]] Arch arch() const noexcept { return arch_; }

  [[nodiscard]] OS os() const noexcept { return os_; }
};

} // namespace nicole

#endif // OPTIONS_H
