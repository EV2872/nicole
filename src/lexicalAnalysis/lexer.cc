#include "../../inc/lexicalAnalysis/lexer.h"

#include <fstream>
#include <iostream>

namespace nicole {

auto Lexer::concatCategories() const noexcept -> const Category {
  std::string pattern{""};
  for (const Category &category : categories_) {
    pattern += category.rawPattern() + "|";
  }
  pattern.pop_back();
  return Category{TokenType::ALL, pattern, false};
}

auto Lexer::checkUnmatched(const std::vector<Token> &tokens) const noexcept
    -> const std::expected<void, Error> {
  bool unmatchedFlag{false};
  std::string everyUnmatched{"Unmatched tokens:\n"};
  for (const Token &TOKEN : tokens) {
    if (TOKEN.type() == TokenType::UNMATCHED) {
      everyUnmatched += TOKEN.raw() + "\n";
      unmatchedFlag = true;
    }
  }
  if (unmatchedFlag) {
    return createError(ERROR_TYPE::CHECK_UNMATCHED, everyUnmatched);
  }
  return std::expected<void, Error>{};
}

auto Lexer::readFile(const std::filesystem::path &fileName) const noexcept
    -> const std::expected<std::string, Error> {
  const std::regex fileNameFormat{"[a-zA-Z]+[a-zA-Z0-9]*\\." + extension_,
                                  std::regex_constants::optimize};
  // method fileName returns just the file
  if (!std::regex_match(fileName.filename().string(), fileNameFormat)) {
    return createError(ERROR_TYPE::FILE_EXTENSION,
                       "The file " + fileName.string() +
                           " does not have extension: " + extension_);
  }
  std::fstream file{fileName};
  if (!file.is_open()) {
    const std::string strErr{"The file " + fileName.string() + " is not open"};
    return createError(ERROR_TYPE::FILE_NOT_OPEN,
                       "The file " + fileName.string() + " is not open");
  }
  std::string text{""};
  std::string line{""};
  while (getline(file, line)) {
    text += line + "\n";
  }
  file.close();
  return text;
}

auto Lexer::analyze(const std::filesystem::path &fileName,
                    bool verbose) const noexcept
    -> const std::expected<TokenStream, Error> {
  const std::expected<std::string, Error> TEXT{readFile(fileName)};
  if (!TEXT) {
    return createError(TEXT.error());
  }
  const Category expression{concatCategories()};
  std::vector<Token> result{};
  std::string::const_iterator start, end, lastMatchEnd;
  start = TEXT->begin();
  end = TEXT->end();
  lastMatchEnd = start;
  std::match_results<std::string::const_iterator> what;
  std::size_t row{1};
  std::size_t col{1};
  while (std::regex_search(start, end, what, expression.pattern().pattern())) {
    if (lastMatchEnd != what[0].first) {
      const std::string UNMATCHED{lastMatchEnd, what[0].first};
      if (verbose) {
        std::cout << "Unmatched: " << UNMATCHED << "\n";
      }

      for (std::string::const_iterator it = lastMatchEnd; it != what[0].first;
           ++it) {
        if (*it == '\n') {
          ++row;
          col = 1;
        } else {
          ++col;
        }
      }

      result.push_back(
          Token{TokenType::UNMATCHED, UNMATCHED, Location{fileName, row, col}});
    }
    for (const Category &category : categories_) {
      if (std::regex_match(what[0].str(), category.pattern().pattern())) {
        if (verbose) {
          std::cout << "Category: " << tokenTypeToString(category.type())
                    << ", Match: " << what[0].str() << " " << row << ":" << col
                    << "\n";
        }
        if (!category.skip()) {
          result.push_back(Token{category.type(), what[0].str(),
                                 Location{fileName, row, col}});
        }
        break;
      }
    }

    for (std::string::const_iterator it = what[0].first; it != what[0].second;
         ++it) {
      if (*it == '\n') {
        ++row;
        col = 1;
      } else {
        ++col;
      }
    }

    lastMatchEnd = what[0].second; // Update last match end position
    start = what[0].second;        // update search position
  }
  // Check for unmatched tokens after the last match
  if (lastMatchEnd != end) {
    const std::string UNMATCHED{lastMatchEnd, end};
    if (verbose) {
      std::cout << "Unmatched: " << UNMATCHED << "\n";
    }

    for (std::string::const_iterator it = lastMatchEnd; it != end; ++it) {
      if (*it == '\n') {
        ++row;
        col = 1;
      } else {
        ++col;
      }
    }

    result.push_back(
        Token{TokenType::UNMATCHED, UNMATCHED, Location{fileName, row, col}});
  }
  const std::expected<void, Error> check{checkUnmatched(result)};
  if (!check) {
    return createError(check.error());
  }
  return TokenStream{result};
}

} // namespace nicole