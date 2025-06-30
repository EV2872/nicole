#ifndef TOKEN_STREAM_H
#define TOKEN_STREAM_H

#include "../errors.h"
#include "token.h"
#include "type.h"
#include <cstddef>
#include <expected>
#include <limits>
#include <variant>
#include <vector>

namespace nicole {

// represents the tokens returned by the lexer
class TokenStream final {
private:
  std::vector<Token> tokens_;
  std::size_t currentPos_{0};

public:
  explicit TokenStream(const std::vector<Token> &tokens) noexcept
      : tokens_{tokens} {}

  [[nodiscard]] auto size() const noexcept -> std::size_t {
    return tokens_.size();
  };

  [[nodiscard]] auto eat() noexcept -> std::expected<std::monostate, Error>;

  [[nodiscard]] auto currentPos() const noexcept -> std::size_t {
    return currentPos_;
  }

  [[nodiscard]] auto isEnd() const noexcept -> bool;

  [[nodiscard]] auto current() const noexcept -> std::expected<Token, Error>;

  [[nodiscard]] auto lookAhead(const size_t pos) const noexcept
      -> std::expected<Token, Error>;

  [[nodiscard]] auto lastRead() const noexcept -> std::expected<Token, Error>;

  [[nodiscard]] auto isCurrentTokenType(const TokenType type) const noexcept
      -> std::expected<bool, Error>;

  [[nodiscard]] auto
  isTokenAheadBeforeSemicolon(const TokenType type) const noexcept -> bool;

  // needed for whenever an import is found
  [[nodiscard]] auto
  insertAfter(const TokenStream &tkStream,
              const size_t pos = std::numeric_limits<size_t>::infinity()) noexcept
      -> std::expected<std::monostate, Error>;

  auto shiftToSemicolon() noexcept -> void;

  [[nodiscard]] auto begin() const noexcept -> decltype(tokens_.begin()) {
    return tokens_.begin();
  }

  [[nodiscard]] auto end() const noexcept -> decltype(tokens_.end()) {
    return tokens_.end();
  }

  [[nodiscard]] auto hasMatchingPairBefore(TokenType open,
                                           TokenType until) const noexcept
      -> bool;
};

} // namespace nicole

#endif