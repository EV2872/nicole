#ifndef TYPE_TABLE_H
#define TYPE_TABLE_H

#include "../../errors.h"
#include "../../lexicalAnalysis/type.h"
#include "types/basicTypes/basicTypes.h"
#include "types/specialTypes/breakType.h"
#include "types/specialTypes/constType.h"
#include "types/specialTypes/noPropagateType.h"
#include "types/specialTypes/nullType.h"
#include "types/specialTypes/ptrType.h"
#include "types/specialTypes/vectorType.h"
#include "types/specialTypes/voidType.h"
#include "types/type.h"
#include "types/userTypes/enumType.h"
#include "types/userTypes/genericInstanceType.h"
#include "types/userTypes/genericParameter.h"
#include "types/userTypes/placeHolder.h"
#include "types/userTypes/userType.h"
#include <expected>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace nicole {

class TypeTable {
private:
  std::unordered_map<std::string, std::shared_ptr<Type>> table_{
      {"bool", std::make_shared<BasicType>(BasicKind::Bool)},
      {"int", std::make_shared<BasicType>(BasicKind::Int)},
      {"float", std::make_shared<BasicType>(BasicKind::Float)},
      {"double", std::make_shared<BasicType>(BasicKind::Double)},
      {"char", std::make_shared<BasicType>(BasicKind::Char)},
      {"str", std::make_shared<BasicType>(BasicKind::Str)},
      {"void", std::make_shared<VoidType>()},
  };

  std::shared_ptr<NullType> null_{std::make_shared<NullType>()};

  std::shared_ptr<NoPropagateType> noPropagate_{
      std::make_shared<NoPropagateType>()};

  std::shared_ptr<BreakType> breakType_{std::make_shared<BreakType>()};

  [[nodiscard]] auto canAssignImpl(const std::shared_ptr<Type> &target,
                                   const std::shared_ptr<Type> &source,
                                   bool pointerContext) const noexcept -> bool;

public:
  [[nodiscard]] auto null() const noexcept
      -> const std::shared_ptr<NullType> & {
    return null_;
  }

  [[nodiscard]] auto boolType() const noexcept
      -> const std::shared_ptr<Type> & {
    return table_.at("bool");
  }

  [[nodiscard]] auto intType() const noexcept
      -> const std::shared_ptr<Type> & {
    return table_.at("int");
  }

  [[nodiscard]] auto floatType() const noexcept
      -> const std::shared_ptr<Type> & {
    return table_.at("float");
  }

  [[nodiscard]] auto doubleType() const noexcept
      -> const std::shared_ptr<Type> & {
    return table_.at("double");
  }

  [[nodiscard]] auto charType() const noexcept
      -> const std::shared_ptr<Type> & {
    return table_.at("char");
  }

  [[nodiscard]] auto strType() const noexcept
      -> const std::shared_ptr<Type> & {
    return table_.at("str");
  }

  [[nodiscard]] auto voidType() const noexcept
      -> const std::shared_ptr<Type> & {
    return table_.at("void");
  }

  [[nodiscard]] auto noPropagateType() const noexcept
      -> const std::shared_ptr<NoPropagateType> & {
    return noPropagate_;
  }

  [[nodiscard]] auto breakType() const noexcept
      -> const std::shared_ptr<BreakType> & {
    return breakType_;
  }

  [[nodiscard]] auto has(const std::string &id) const noexcept -> bool;

  [[nodiscard]] auto getType(const std::string &id) const noexcept
      -> const std::expected<std::shared_ptr<Type>, Error>;

  [[nodiscard]] auto insert(const std::shared_ptr<Type> &type) noexcept
      -> std::expected<std::monostate, Error>;

  [[nodiscard]] auto
  isPossibleType(const std::shared_ptr<Type> &type) const noexcept -> bool;

  [[nodiscard]] auto
  isGenericType(const std::shared_ptr<Type> &type,
                const std::vector<GenericParameter> &generics) const noexcept
      -> bool;

  [[nodiscard]] auto
  isCompundEnumType(const std::shared_ptr<Type> &type) const noexcept
      -> std::expected<std::shared_ptr<Type>, Error>;

  [[nodiscard]] auto
  isCompundUserType(const std::shared_ptr<Type> &type) const noexcept
      -> std::expected<std::shared_ptr<Type>, Error>;

  [[nodiscard]] auto
  isCompundGenericType(const std::shared_ptr<Type> &type,
                       const std::vector<GenericParameter> &list) const noexcept
      -> std::expected<std::shared_ptr<Type>, Error>;

  [[nodiscard]] auto
  isCompundPlaceHolder(const std::shared_ptr<Type> &type) const noexcept
      -> bool;

  [[nodiscard]] auto
  areSameType(const std::shared_ptr<Type> &type1,
              const std::shared_ptr<Type> &type2) const noexcept -> bool;

  [[nodiscard]] auto
  canAssign(const std::shared_ptr<Type> &target,
            const std::shared_ptr<Type> &source) const noexcept -> bool;

  [[nodiscard]] auto
  haveCommonAncestor(const std::shared_ptr<Type> &type1,
                     const std::shared_ptr<Type> &type2) const noexcept -> bool;

  [[nodiscard]] auto applyUnaryOperator(const std::shared_ptr<Type> &operand,
                                        const TokenType op) const noexcept
      -> std::expected<std::shared_ptr<Type>, Error>;

  [[nodiscard]] auto applyBinaryOperator(const std::shared_ptr<Type> &left,
                                         const std::shared_ptr<Type> &right,
                                         TokenType op) const noexcept
      -> std::expected<std::shared_ptr<Type>, Error>;

  [[nodiscard]] auto applyGenericReplacements(
      const std::shared_ptr<Type> &type,
      const std::vector<GenericParameter> &genericParams,
      const std::vector<std::shared_ptr<Type>> &replacements) const noexcept
      -> std::expected<std::shared_ptr<Type>, Error>;
};

} // namespace nicole

#endif