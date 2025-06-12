#include "../../inc/lexicalAnalysis/tokenStream.h"

namespace nicole {

auto TokenStream::eat() noexcept -> std::expected<std::monostate, Error> {
  if (currentPos_ < tokens_.size()) {
    ++currentPos_;
    return std::expected<std::monostate, Error>{std::monostate{}};
  }
  return createError(ERROR_TYPE::EAT, "invalid access to tokens while eating");
}

auto TokenStream::isEnd() const noexcept -> bool {
  return currentPos_ == tokens_.size();
}

auto TokenStream::current() const noexcept -> std::expected<Token, Error> {
  if (currentPos_ < tokens_.size())
    return tokens_[currentPos_];
  return createError(ERROR_TYPE::CURRENT, "invalid access to tokens");
}

auto TokenStream::lookAhead(const size_t pos) const noexcept
    -> std::expected<Token, Error> {
  if (currentPos_ + pos < tokens_.size())
    return tokens_[currentPos_ + pos];
  return createError(ERROR_TYPE::LOOK_AHEAD, "invalid access to tokens");
}

auto TokenStream::lastRead() const noexcept -> std::expected<Token, Error> {
  if (currentPos_ - 1 < tokens_.size())
    return tokens_[currentPos_ - 1];
  return createError(ERROR_TYPE::LAST_READ, "invalid access to tokens");
}

auto TokenStream::isCurrentTokenType(const TokenType type) const noexcept
    -> std::expected<bool, Error> {
  if (currentPos_ < tokens_.size())
    return tokens_[currentPos_].type() == type;
  return createError(ERROR_TYPE::IS_CURRENT_TOKEN_TYPE,
                     "invalid access to tokens");
}

auto TokenStream::isTokenAheadBeforeSemicolon(
    const TokenType type) const noexcept -> bool {
  bool foundToken{false};
  for (size_t i{currentPos_}; i < tokens_.size(); ++i) {
    Token tk{tokens_[i]};
    if (tk.type() == type) {
      foundToken = true;
      break;
    }
    if (tk.type() == TokenType::SEMICOLON) {
      break;
    }
  }
  return foundToken;
}

auto TokenStream::insertAfter(const TokenStream &tkStream,
                              const size_t pos) noexcept
    -> std::expected<std::monostate, Error> {
  if (pos == std::numeric_limits<int>::infinity()) {
    return createError(ERROR_TYPE::INSERT_AFTER,
                       "cannot insert after the given position");
  }
  tokens_.insert(tokens_.begin() + static_cast<long>(pos), tkStream.begin(),
                 tkStream.end());
  return std::expected<std::monostate, Error>{std::monostate{}};
}

auto TokenStream::shiftToSemicolon() noexcept -> void {
  while (currentPos_ < size()) {
    if (tokens_[currentPos_].type() == TokenType::SEMICOLON) {
      break;
    }
    ++currentPos_;
  }
}

auto TokenStream::hasMatchingPairBefore(TokenType open,
                                        TokenType until) const noexcept
    -> bool {
  size_t depth = 0;
  size_t i = currentPos_;
  while (i < tokens_.size()) {
    const TokenType type = tokens_[i].type();
    if (type == until)
      return false; // llegó al delimitador sin cerrar

    if (type == open) {
      ++depth;
    } else if ((open == TokenType::OPERATOR_SMALLER &&
                type == TokenType::OPERATOR_GREATER) ||
               (open == TokenType::LP && type == TokenType::RP) ||
               (open == TokenType::LC && type == TokenType::RC) ||
               (open == TokenType::LB && type == TokenType::RB)) {
      if (depth == 0)
        return false; // cierre sin apertura
      --depth;
      if (depth == 0)
        return true; // se cerró correctamente
    }

    ++i;
  }
  return false;
}

} // namespace nicole