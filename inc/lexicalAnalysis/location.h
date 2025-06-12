#ifndef LOCATION_H
#define LOCATION_H

#include <filesystem>

namespace nicole {

// Represents the position of the token
class Location final {
private:
  std::filesystem::path file_;
  std::size_t row_;
  std::size_t col_;

public:
  explicit Location(const std::filesystem::path &file, std::size_t row,
                    std::size_t col) noexcept
      : file_{file}, row_{row}, col_{col} {};

  [[nodiscard]] auto file() const noexcept -> const std::filesystem::path & {
    return file_;
  }

  [[nodiscard]] auto row() const noexcept -> std::size_t { return row_; }

  [[nodiscard]] auto col() const noexcept -> std::size_t { return col_; }

  [[nodiscard]] auto operator<(const Location &loc) const noexcept -> bool {
    if (row_ < loc.row_) {
      return true;
    }
    if (col_ < loc.col_) {
      return true;
    }
    return false;
  }
};

} // namespace nicole

#endif