#ifndef LEXER_H
#define LEXER_H

#include <filesystem>
#include <vector>

#include "category.h"
#include "tokenStream.h"

namespace nicole {

// Lexer that tokenize an input file
class Lexer final {
private:
  std::vector<Category> categories_;

  std::string extension_;

  // makes a category that matches with every token of our sintax
  [[nodiscard]] auto concatCategories() const noexcept -> const Category;

  [[nodiscard]] auto
  readFile(const std::filesystem::path &fileName) const noexcept
      -> const std::expected<std::string, Error>;

  [[nodiscard]] auto
  checkUnmatched(const std::vector<Token> &tokens) const noexcept
      -> const std::expected<void, Error>;

public:
  explicit Lexer(const std::vector<Category> &categories,
                 const std::string &extension) noexcept
      : categories_{categories}, extension_{extension} {};

  [[nodiscard]] auto categories() const noexcept
      -> const std::vector<Category> & {
    return categories_;
  }

  [[nodiscard]] auto analyze(const std::filesystem::path &fileName,
                             bool verbose = false) const noexcept
      -> const std::expected<TokenStream, Error>;
};

} // namespace nicole

#endif