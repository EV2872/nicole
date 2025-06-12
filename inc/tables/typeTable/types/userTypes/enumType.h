#ifndef ENUM_TYPE_H
#define ENUM_TYPE_H

#include "../type.h"
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

namespace nicole {

class EnumType final : public Type {
private:
  std::string name_;
  std::vector<std::string> values_;

public:
  EnumType(const std::string &name,
           const std::vector<std::string> &values) noexcept
      : name_(name), values_(values) {}

  [[nodiscard]] auto name() const noexcept -> const std::string & {
    return name_;
  }

  [[nodiscard]] auto values() const noexcept
      -> const std::vector<std::string> & {
    return values_;
  }

  [[nodiscard]] auto hasIdentifier(const std::string &identifier) const noexcept
      -> bool {
    return std::find(values_.begin(), values_.end(), identifier) !=
           values_.end();
  }

  [[nodiscard]] auto
  identifierToNumber(const std::string &identifier) const noexcept
      -> std::expected<std::size_t, Error> {
    for (size_t i{0}; i < values_.size(); ++i) {
      if (identifier == values_[i]) {
        return i;
      }
    }
    return createError(ERROR_TYPE::TYPE,
                       "unable to convert to number identifier " + identifier +
                           "in enum: " + name_);
  }

  [[nodiscard]] auto toString() const noexcept -> std::string override {
    std::ostringstream oss;
    oss << "enum " << name_ << " { ";
    for (size_t i = 0; i < values_.size(); ++i) {
      oss << values_[i];
      if (i != values_.size() - 1) {
        oss << ", ";
      }
    }
    oss << " }";
    return oss.str();
  }

  [[nodiscard]] auto llvmVersion(llvm::LLVMContext &context) const noexcept
      -> std::expected<llvm::Type *, Error> override {
    // Representamos enum como i32
    return llvm::Type::getInt32Ty(context);
  }
};

} // namespace nicole

#endif
