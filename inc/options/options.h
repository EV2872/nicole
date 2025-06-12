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

  [[nodiscard]] auto optimize() const noexcept -> bool { return optimize_; }

  [[nodiscard]] auto printTree() const noexcept -> bool { return printTree_; }

  [[nodiscard]] auto printIR() const noexcept -> bool { return printIR_; }

  [[nodiscard]] auto validateTree() const noexcept -> bool {
    return validateTree_;
  }

  [[nodiscard]] auto justInTime() const noexcept -> bool { return justInTime_; }

  [[nodiscard]] auto useLLD() const noexcept -> bool { return useLLD_; }

  [[nodiscard]] auto binaryName() const noexcept -> const std::string & {
    return binaryName_;
  }

  [[nodiscard]] auto entryFilePath() const noexcept
      -> const std::filesystem::path & {
    return entryFilePath_;
  }

  [[nodiscard]] auto arch() const noexcept -> Arch { return arch_; }

  [[nodiscard]] auto os() const noexcept -> OS { return os_; }
};

} // namespace nicole

#endif // OPTIONS_H
