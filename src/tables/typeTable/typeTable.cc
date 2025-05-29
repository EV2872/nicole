#include "../../../inc/tables/typeTable/typeTable.h"
#include <iostream>
#include <memory>
#include <vector>

namespace nicole {

bool TypeTable::has(const std::string &id) const noexcept {
  return table_.count(id);
}

const std::expected<std::shared_ptr<Type>, Error>
TypeTable::getType(const std::string &id) const noexcept {
  if (has(id)) {
    return table_.at(id);
  }
  return createError(ERROR_TYPE::TYPE, "the type: " + id + " does not exists");
}

std::expected<std::monostate, Error>
TypeTable::insert(const std::shared_ptr<Type> &type) noexcept {
  if (has(type->toString())) {
    return createError(ERROR_TYPE::TYPE,
                       "the type: " + type->toString() + " already exists");
  }
  if (const auto &enumType = std::dynamic_pointer_cast<EnumType>(type)) {
    table_[enumType->name()] = type;
    return {};
  }
  const auto &userType = std::dynamic_pointer_cast<UserType>(type);
  table_[userType->name()] = type;
  return {};
}

bool TypeTable::isPossibleType(
    const std::shared_ptr<Type> &type) const noexcept {
  if (const auto &basicType = std::dynamic_pointer_cast<BasicType>(type)) {
    return true;
  } else if (const auto &voidType = std::dynamic_pointer_cast<VoidType>(type)) {
    return true;
  } else if (const auto &vectorType =
                 std::dynamic_pointer_cast<VectorType>(type)) {
    return isPossibleType(vectorType->elementType());
  } else if (const auto &pointerType =
                 std::dynamic_pointer_cast<PointerType>(type)) {
    return isPossibleType(pointerType->baseType());
  } else if (const auto &constType =
                 std::dynamic_pointer_cast<ConstType>(type)) {
    return isPossibleType(constType->baseType());
  } else if (const auto &nullType = std::dynamic_pointer_cast<NullType>(type)) {
    return true;
  } else if (const auto &enumType = std::dynamic_pointer_cast<EnumType>(type)) {
    if (!has(enumType->name())) {
      return false;
    }
    return true;
  } else if (const auto &genericInstanceType =
                 std::dynamic_pointer_cast<GenericInstanceType>(type)) {
    if (!has(genericInstanceType->name())) {
      return false;
    }
    for (const auto &arg : genericInstanceType->typeArgs()) {
      if (!isPossibleType(arg)) {
        return false;
      }
    }
    return true;
  } else if (const auto &userType = std::dynamic_pointer_cast<UserType>(type)) {
    if (!has(userType->name())) {
      return false;
    }
    return true;
  }
  return false;
}

bool TypeTable::isGenericType(
    const std::shared_ptr<Type> &type,
    const std::vector<GenericParameter> &generics) const noexcept {
  if (std::dynamic_pointer_cast<PlaceHolder>(type))
    return true;
  if (const auto &vectorType = std::dynamic_pointer_cast<VectorType>(type)) {
    return isGenericType(vectorType->elementType(), generics);
  } else if (const auto &pointerType =
                 std::dynamic_pointer_cast<PointerType>(type)) {
    return isGenericType(pointerType->baseType(), generics);
  } else if (const auto &constType =
                 std::dynamic_pointer_cast<ConstType>(type)) {
    return isGenericType(constType->baseType(), generics);
  } else if (const auto &enumType = std::dynamic_pointer_cast<EnumType>(type)) {
    for (const auto &generic : generics) {
      if (generic.name() == enumType->name()) {
        return true;
      }
    }
    return false;
  } else if (const auto &genericInstanceType =
                 std::dynamic_pointer_cast<GenericInstanceType>(type)) {
    if (!has(genericInstanceType->name())) {
      return false;
    }
    for (const auto &arg : genericInstanceType->typeArgs()) {
      if (!isGenericType(arg, generics)) {
        return false;
      }
    }
    return true;
  } else if (const auto &userType = std::dynamic_pointer_cast<UserType>(type)) {
    for (const auto &generic : generics) {
      if (generic.name() == userType->name()) {
        return true;
      }
    }
    return false;
  }
  return false;
}

std::expected<std::shared_ptr<Type>, Error>
TypeTable::isCompundUserType(const std::shared_ptr<Type> &type) const noexcept {
  // Const
  if (auto ct = std::dynamic_pointer_cast<ConstType>(type)) {
    auto inner = isCompundUserType(ct->baseType());
    if (!inner) return createError(inner.error());
    return std::make_shared<ConstType>(inner.value());
  }

  // Puntero
  if (auto pt = std::dynamic_pointer_cast<PointerType>(type)) {
    auto inner = isCompundUserType(pt->baseType());
    if (!inner) return createError(inner.error());
    return std::make_shared<PointerType>(inner.value());
  }

  // Vector
  if (auto vt = std::dynamic_pointer_cast<VectorType>(type)) {
    auto inner = isCompundUserType(vt->elementType());
    if (!inner) return createError(inner.error());
    return std::make_shared<VectorType>(inner.value());
  }

  // Instancia genérica de un UserType
  if (auto git = std::dynamic_pointer_cast<GenericInstanceType>(type)) {
    auto exists = getType(git->name());
    if (!exists) return createError(exists.error());
    // Asegurarse de que la definición base es un UserType
    if (!std::dynamic_pointer_cast<UserType>(exists.value()))
      return createError(ERROR_TYPE::TYPE,
                         "El tipo encontrado no es un UserType");
    // No deshacemos los argumentos genéricos, devolvemos la misma instancia
    return git;
  }

  // Tipo definido por el usuario
  if (auto ut = std::dynamic_pointer_cast<UserType>(type)) {
    auto exists = getType(ut->name());
    if (!exists) return createError(exists.error());
    if (!std::dynamic_pointer_cast<UserType>(exists.value()))
      return createError(ERROR_TYPE::TYPE,
                         "El tipo encontrado no es un UserType");
    return exists.value();
  }

  // No es un tipo de usuario compuesto
  return createError(ERROR_TYPE::TYPE,
                     "El tipo no es un tipo compuesto de usuario");
}


bool TypeTable::isCompundPlaceHolder(
    const std::shared_ptr<Type> &type) const noexcept {
  if (!type)
    return false;
  if (std::dynamic_pointer_cast<PlaceHolder>(type))
    return true;
  if (auto ct = std::dynamic_pointer_cast<ConstType>(type))
    return isCompundPlaceHolder(ct->baseType());
  if (auto pt = std::dynamic_pointer_cast<PointerType>(type))
    return isCompundPlaceHolder(pt->baseType());
  if (auto vt = std::dynamic_pointer_cast<VectorType>(type))
    return isCompundPlaceHolder(vt->elementType());
  if (auto git = std::dynamic_pointer_cast<GenericInstanceType>(type)) {
    for (const auto &arg : git->typeArgs()) {
      if (isCompundPlaceHolder(arg))
        return true;
    }
    return false;
  }
  return false;
}

std::expected<std::shared_ptr<Type>, Error>
TypeTable::isCompundEnumType(const std::shared_ptr<Type> &type) const noexcept {
  if (auto constType = std::dynamic_pointer_cast<ConstType>(type)) {
    auto baseRes = isCompundEnumType(constType->baseType());
    if (!baseRes)
      return createError(baseRes.error());
    return std::make_shared<ConstType>(baseRes.value());
  }
  if (auto pointerType = std::dynamic_pointer_cast<PointerType>(type)) {
    auto baseRes = isCompundEnumType(pointerType->baseType());
    if (!baseRes)
      return createError(baseRes.error());
    return std::make_shared<PointerType>(baseRes.value());
  }
  if (auto vectorType = std::dynamic_pointer_cast<VectorType>(type)) {
    auto baseRes = isCompundEnumType(vectorType->elementType());
    if (!baseRes)
      return createError(baseRes.error());
    return std::make_shared<VectorType>(baseRes.value());
  }
  if (auto genericInstanceType =
          std::dynamic_pointer_cast<GenericInstanceType>(type)) {
    auto exists = getType(genericInstanceType->name());
    if (!exists)
      return createError(exists.error());
    const auto generciList{genericInstanceType->typeArgs()};
    for (std::size_t i{0}; i < generciList.size(); ++i) {
      auto maskedEnum = isCompundEnumType(generciList[i]);
      if (maskedEnum) {
        std::cout << i << " " << maskedEnum.value()->toString();
        const auto replaced{
            genericInstanceType->setGenericReplacement(i, *maskedEnum)};
        if (!replaced) {
          return createError(replaced.error());
        }
      }
    }
    return genericInstanceType;
  }
  if (auto userType = std::dynamic_pointer_cast<UserType>(type)) {
    auto exists = getType(userType->name());
    if (!exists)
      return createError(exists.error());
    if (auto enumType = std::dynamic_pointer_cast<EnumType>(exists.value()))
      return enumType;
    else
      return createError(ERROR_TYPE::TYPE,
                         "El tipo encontrado no es un EnumType");
  }
  return createError(ERROR_TYPE::TYPE, "El tipo no es un Enum compuesto");
}

std::expected<std::shared_ptr<Type>, Error> TypeTable::isCompundGenericType(
    const std::shared_ptr<Type> &type,
    const std::vector<GenericParameter> &genericList) const noexcept {
  if (auto constType = std::dynamic_pointer_cast<ConstType>(type)) {
    auto baseRes = isCompundGenericType(constType->baseType(), genericList);
    if (!baseRes)
      return createError(baseRes.error());
    return std::make_shared<ConstType>(baseRes.value());
  }

  if (auto pointerType = std::dynamic_pointer_cast<PointerType>(type)) {
    auto baseRes = isCompundGenericType(pointerType->baseType(), genericList);
    if (!baseRes)
      return createError(baseRes.error());
    return std::make_shared<PointerType>(baseRes.value());
  }

  if (auto vectorType = std::dynamic_pointer_cast<VectorType>(type)) {
    auto baseRes = isCompundGenericType(vectorType->elementType(), genericList);
    if (!baseRes)
      return createError(baseRes.error());
    return std::make_shared<VectorType>(baseRes.value());
  }

  if (auto genericInstanceType =
          std::dynamic_pointer_cast<GenericInstanceType>(type)) {
    for (const auto &gp : genericList) {
      if (gp.name() == genericInstanceType->name()) {
        return std::make_shared<PlaceHolder>(gp);
      }
    }
    const auto typeArgs = genericInstanceType->typeArgs();
    for (std::size_t i = 0; i < typeArgs.size(); ++i) {
      auto masked = isCompundGenericType(typeArgs[i], genericList);
      if (!masked)
        return createError(masked.error());
      auto replaced = genericInstanceType->setGenericReplacement(i, *masked);
      if (!replaced)
        return createError(replaced.error());
    }
    return genericInstanceType;
  }

  if (auto userType = std::dynamic_pointer_cast<UserType>(type)) {
    for (const auto &gp : genericList) {
      if (gp.name() == userType->name()) {
        return std::make_shared<PlaceHolder>(gp);
      }
    }
    return type;
  }
  return createError(ERROR_TYPE::TYPE,
                     "El tipo no es un tipo compuesto genérico");
}

bool TypeTable::areSameType(const std::shared_ptr<Type> &type1,
                            const std::shared_ptr<Type> &type2) const noexcept {
  // Si ambos apuntan al mismo objeto, son iguales.
  if (type1 == type2) {
    return true;
  }
  // Si alguno es nulo, no son iguales.
  if (!type1 || !type2) {
    return false;
  }

  // Caso BasicType
  if (auto basic1 = std::dynamic_pointer_cast<BasicType>(type1)) {
    if (auto basic2 = std::dynamic_pointer_cast<BasicType>(type2)) {
      return basic1->baseKind() == basic2->baseKind();
    }
    return false;
  }

  // Caso VoidType
  if (std::dynamic_pointer_cast<VoidType>(type1)) {
    return std::dynamic_pointer_cast<VoidType>(type2) != nullptr;
  }

  // Caso NullType
  if (std::dynamic_pointer_cast<NullType>(type1)) {
    return std::dynamic_pointer_cast<NullType>(type2) != nullptr;
  }

  // Caso VectorType
  if (auto vec1 = std::dynamic_pointer_cast<VectorType>(type1)) {
    if (auto vec2 = std::dynamic_pointer_cast<VectorType>(type2)) {
      return areSameType(vec1->elementType(), vec2->elementType());
    }
    return false;
  }

  // Caso PointerType
  if (auto ptr1 = std::dynamic_pointer_cast<PointerType>(type1)) {
    if (auto ptr2 = std::dynamic_pointer_cast<PointerType>(type2)) {
      return areSameType(ptr1->baseType(), ptr2->baseType());
    }
    return false;
  }

  // Caso ConstType
  if (auto const1 = std::dynamic_pointer_cast<ConstType>(type1)) {
    if (auto const2 = std::dynamic_pointer_cast<ConstType>(type2)) {
      return areSameType(const1->baseType(), const2->baseType());
    }
    return false;
  }

  // Caso EnumType
  if (auto enum1 = std::dynamic_pointer_cast<EnumType>(type1)) {
    if (auto enum2 = std::dynamic_pointer_cast<EnumType>(type2)) {
      return enum1->name() == enum2->name();
    }
    return false;
  }

  // Caso GenericInstanceType
  if (auto genInst1 = std::dynamic_pointer_cast<GenericInstanceType>(type1)) {
    if (auto genInst2 = std::dynamic_pointer_cast<GenericInstanceType>(type2)) {
      // Se comparan primero los tipos genéricos base
      if (genInst1->name() != genInst2->name()) {
        return false;
      }
      const auto &args1 = genInst1->typeArgs();
      const auto &args2 = genInst2->typeArgs();
      if (args1.size() != args2.size()) {
        return false;
      }
      for (size_t i = 0; i < args1.size(); ++i) {
        if (!areSameType(args1[i], args2[i])) {
          return false;
        }
      }
      return true;
    }
    return false;
  }

  // Caso UserType
  if (auto user1 = std::dynamic_pointer_cast<UserType>(type1)) {
    if (auto user2 = std::dynamic_pointer_cast<UserType>(type2)) {
      return user1->name() == user2->name();
    }
    return false;
  }

  // Si no se reconoce el tipo, se compara mediante su representación canónica.
  return type1->toString() == type2->toString();
}

bool TypeTable::canAssign(const std::shared_ptr<Type> &target,
                          const std::shared_ptr<Type> &source) const noexcept {
  return canAssignImpl(target, source, false);
}

bool TypeTable::canAssignImpl(const std::shared_ptr<Type> &target,
                              const std::shared_ptr<Type> &source,
                              bool pointerContext) const noexcept {
  if (areSameType(target, source))
    return true;

  if (std::dynamic_pointer_cast<NullType>(source)) {
    if (std::dynamic_pointer_cast<PointerType>(target))
      return true;
  }

  std::shared_ptr<Type> tTarget = target;
  std::shared_ptr<Type> tSource = source;
  if (auto constTarget = std::dynamic_pointer_cast<ConstType>(target))
    tTarget = constTarget->baseType();
  if (auto constSource = std::dynamic_pointer_cast<ConstType>(source))
    tSource = constSource->baseType();

  if (auto targetPtr = std::dynamic_pointer_cast<PointerType>(tTarget)) {
    if (auto sourcePtr = std::dynamic_pointer_cast<PointerType>(tSource))
      return canAssignImpl(targetPtr->baseType(), sourcePtr->baseType(), true);
  }

  if (auto holderTarget = std::dynamic_pointer_cast<PlaceHolder>(target))
    return true;
  if (auto holderSource = std::dynamic_pointer_cast<PlaceHolder>(source))
    return true;

  auto targetGenInst = std::dynamic_pointer_cast<GenericInstanceType>(tTarget);
  auto sourceGenInst = std::dynamic_pointer_cast<GenericInstanceType>(tSource);
  if (targetGenInst && sourceGenInst) {
    if (targetGenInst->name() != sourceGenInst->name())
      return false;
    const auto &argsTarget = targetGenInst->typeArgs();
    const auto &argsSource = sourceGenInst->typeArgs();
    if (argsTarget.size() != argsSource.size())
      return false;
    for (size_t i = 0; i < argsTarget.size(); ++i) {
      if (!canAssignImpl(argsTarget[i], argsSource[i], pointerContext))
        return false;
    }
    return true;
  }

  if (auto targetUser = std::dynamic_pointer_cast<UserType>(tTarget)) {
    if (auto sourceUser = std::dynamic_pointer_cast<UserType>(tSource)) {
      // Se permite A = A sin importar el contexto.
      if (targetUser->name() == sourceUser->name())
        return true;
      // Fuera del contexto de punteros no se admite la asignación basada en
      // herencia.
      if (!pointerContext)
        return false;
      // En contexto de punteros se permite que B herede de A.
      for (auto current = sourceUser; current; current = current->baseType()) {
        if (current->name() == targetUser->name())
          return true;
      }
    }
  }

  if (auto targetBasic = std::dynamic_pointer_cast<BasicType>(tTarget)) {
    if (auto sourceBasic = std::dynamic_pointer_cast<BasicType>(tSource))
      return targetBasic->baseKind() == sourceBasic->baseKind();
  }

  if (auto targetEnum = std::dynamic_pointer_cast<EnumType>(tTarget)) {
    if (auto sourceEnum = std::dynamic_pointer_cast<EnumType>(tSource))
      return targetEnum->name() == sourceEnum->name();
  }

  if (auto targetVec = std::dynamic_pointer_cast<VectorType>(tTarget)) {
    if (auto sourceVec = std::dynamic_pointer_cast<VectorType>(tSource)) {
      if (areSameType(sourceVec->elementType(), null_)) {
        return true;
      }
      return canAssignImpl(targetVec->elementType(), sourceVec->elementType(),
                           pointerContext);
    }
  }

  return false;
}

bool TypeTable::haveCommonAncestor(
    const std::shared_ptr<Type> &type1,
    const std::shared_ptr<Type> &type2) const noexcept {
  // Solo tiene sentido para tipos de usuario (incluyendo GenericInstanceType,
  // que se tratan como UserType)
  auto user1 = std::dynamic_pointer_cast<UserType>(type1);
  auto user2 = std::dynamic_pointer_cast<UserType>(type2);
  if (user1 && user2) {
    // Comprobar si user1 es ancestro de user2
    for (auto current = user2; current; current = current->baseType()) {
      if (current->name() == user1->name()) {
        return true;
      }
    }
    // O si user2 es ancestro de user1
    for (auto current = user1; current; current = current->baseType()) {
      if (current->name() == user2->name()) {
        return true;
      }
    }
  }
  return false;
}

// these two methods are still unfinished
std::expected<std::shared_ptr<Type>, Error>
TypeTable::applyUnaryOperator(const std::shared_ptr<Type> &operand,
                              const TokenType op) const noexcept {
  if (!operand)
    return createError(ERROR_TYPE::TYPE, "operand is null");

  std::shared_ptr<Type> t = operand;
  if (auto constType = std::dynamic_pointer_cast<ConstType>(t))
    t = constType->baseType();

  if (std::dynamic_pointer_cast<PlaceHolder>(t))
    return boolType();

  // Si el operando es un tipo definido por el usuario, se busca el método
  // correspondiente sin parámetros.
  if (auto userType = std::dynamic_pointer_cast<UserType>(t)) {
    std::string methodName;
    switch (op) {
    case TokenType::OPERATOR_NOT:
      methodName = "notable";
      break;
    case TokenType::INCREMENT:
      methodName = "incrementable";
      break;
    case TokenType::DECREMENT:
      methodName = "decrementable";
      break;
    case TokenType::OPERATOR_SUB:
      methodName = "negable";
      break;
    default:
      return createError(ERROR_TYPE::TYPE,
                         "unsupported unary operator for user-defined type");
    }
    auto methodsExp = userType->getMethods(methodName);
    if (!methodsExp)
      return createError(methodsExp.error());
    auto methods = methodsExp.value();
    for (const auto &m : methods) {
      if (m.params().size() == 0) {
        // Para operator_not, el resultado debe ser bool.
        if (op == TokenType::OPERATOR_NOT) {
          auto boolTypeExp = boolType();
          if (!areSameType(m.returnType(), boolTypeExp))
            return createError(ERROR_TYPE::TYPE, "operator ! must return bool");
          return boolTypeExp;
        }
        // Para los otros operadores, se retorna el tipo que devuelve el método.
        return m.returnType();
      }
    }
    return createError(ERROR_TYPE::TYPE, "user type " + userType->name() +
                                             " does not support operator " +
                                             tokenTypeToString(op));
  }

  // Si es un PointerType.
  if (auto ptrType = std::dynamic_pointer_cast<PointerType>(t)) {
    switch (op) {
    case TokenType::INCREMENT:
    case TokenType::DECREMENT:
      return t; // Se retorna el mismo tipo puntero.
    case TokenType::OPERATOR_NOT: {
      return boolType();
    }
    default:
      return createError(ERROR_TYPE::TYPE,
                         "unsupported unary operator for pointer type");
    }
  }

  // Si es un BasicType.
  if (auto basic = std::dynamic_pointer_cast<BasicType>(t)) {
    switch (op) {
    case TokenType::OPERATOR_NOT: {
      switch (basic->baseKind()) {
      case BasicKind::Str:
        return createError(ERROR_TYPE::TYPE,
                           "operator " + tokenTypeToString(op) +
                               " cannot be applied to type " + t->toString());
      default:
        return boolType();
      }
    }
    case TokenType::INCREMENT:
    case TokenType::DECREMENT:
    case TokenType::OPERATOR_SUB:
      // Verificar que sea numérico (o char, asumido como numérico).
      switch (basic->baseKind()) {
      case BasicKind::Int:
      case BasicKind::Float:
      case BasicKind::Double:
      case BasicKind::Char:
        return t;
      default:
        return createError(ERROR_TYPE::TYPE,
                           "operator " + tokenTypeToString(op) +
                               " cannot be applied to type " + t->toString());
      }
    default:
      return createError(ERROR_TYPE::TYPE, "unsupported unary operator");
    }
  }

  return createError(ERROR_TYPE::TYPE,
                     "unary operator cannot be applied to type " +
                         t->toString());
}

std::expected<std::shared_ptr<Type>, Error>
TypeTable::applyBinaryOperator(const std::shared_ptr<Type> &leftOperand,
                               const std::shared_ptr<Type> &rightOperand,
                               TokenType operatorToken) const noexcept {
  // Desenrollar los tipos constantes para trabajar con la representación base.
  std::shared_ptr<Type> leftResolvedType = leftOperand;
  std::shared_ptr<Type> rightResolvedType = rightOperand;
  if (auto leftConst = std::dynamic_pointer_cast<ConstType>(leftResolvedType))
    leftResolvedType = leftConst->baseType();
  if (auto rightConst = std::dynamic_pointer_cast<ConstType>(rightResolvedType))
    rightResolvedType = rightConst->baseType();

  // Caso: operador +
  if (operatorToken == TokenType::OPERATOR_ADD) {
    // Caso 1: Operando izquierdo y derecho son BasicType.
    auto leftBasicType = std::dynamic_pointer_cast<BasicType>(leftResolvedType);
    auto rightBasicType =
        std::dynamic_pointer_cast<BasicType>(rightResolvedType);
    if (leftBasicType && rightBasicType) {
      // Si alguno es de tipo cadena (Str), se asume concatenación.
      if (leftBasicType->baseKind() == BasicKind::Str and
          rightBasicType->baseKind() == BasicKind::Str) {
        return strType();
      }
      // Si son numéricos y ambos son del mismo tipo.
      if (areSameType(leftResolvedType, rightResolvedType))
        return leftResolvedType;
      else
        return createError(ERROR_TYPE::TYPE,
                           "incompatible numeric types for operator +");
    }
    // Caso 2: Aritmética de punteros: si el operando izquierdo es un puntero
    // y el derecho es un entero (o un char).
    auto leftPointerType =
        std::dynamic_pointer_cast<PointerType>(leftResolvedType);
    auto rightNumericBasic =
        std::dynamic_pointer_cast<BasicType>(rightResolvedType);
    if (leftPointerType && rightNumericBasic &&
        (rightNumericBasic->baseKind() == BasicKind::Int ||
         rightNumericBasic->baseKind() == BasicKind::Char)) {
      return leftResolvedType;
    }
    return createError(ERROR_TYPE::TYPE,
                       "operator + not applicable for given types: " +
                           leftResolvedType->toString() + " and " +
                           rightResolvedType->toString());
  }

  // Caso: operador -
  if (operatorToken == TokenType::OPERATOR_SUB) {
    // Caso 1: Resta entre BasicType numéricos.
    auto leftNumericType =
        std::dynamic_pointer_cast<BasicType>(leftResolvedType);
    auto rightNumericType =
        std::dynamic_pointer_cast<BasicType>(rightResolvedType);
    if (leftNumericType && rightNumericType) {
      if (areSameType(leftResolvedType, rightResolvedType))
        return leftResolvedType;
      else
        return createError(ERROR_TYPE::TYPE,
                           "incompatible numeric types for operator -");
    }
    // Caso 2: Resta entre punteros.
    auto leftPtrType = std::dynamic_pointer_cast<PointerType>(leftResolvedType);
    if (leftPtrType) {
      auto rightNumericOperand =
          std::dynamic_pointer_cast<BasicType>(rightResolvedType);
      if (rightNumericOperand &&
          (rightNumericOperand->baseKind() == BasicKind::Int ||
           rightNumericOperand->baseKind() == BasicKind::Char)) {
        return leftResolvedType;
      }
      // Si se resta dos punteros, el resultado es un entero.
      auto rightPtrType =
          std::dynamic_pointer_cast<PointerType>(rightResolvedType);
      if (rightPtrType) {
        return intType();
      }
    }
    return createError(ERROR_TYPE::TYPE,
                       "operator - not applicable for given types: " +
                           leftResolvedType->toString() + " and " +
                           rightResolvedType->toString());
  }

  // Caso: operador *
  if (operatorToken == TokenType::OPERATOR_MULT) {
    auto leftNumeric = std::dynamic_pointer_cast<BasicType>(leftResolvedType);
    auto rightNumeric = std::dynamic_pointer_cast<BasicType>(rightResolvedType);
    if (leftNumeric && rightNumeric) {
      if (areSameType(leftResolvedType, rightResolvedType))
        return leftResolvedType;
      else
        return createError(ERROR_TYPE::TYPE,
                           "incompatible types for operator *");
    }
    return createError(ERROR_TYPE::TYPE,
                       "operator * not applicable for given types: " +
                           leftResolvedType->toString() + " and " +
                           rightResolvedType->toString());
  }

  // Caso: operador /
  if (operatorToken == TokenType::OPERATOR_DIV) {
    auto leftNumeric = std::dynamic_pointer_cast<BasicType>(leftResolvedType);
    auto rightNumeric = std::dynamic_pointer_cast<BasicType>(rightResolvedType);
    if (leftNumeric && rightNumeric) {
      if (areSameType(leftResolvedType, rightResolvedType))
        return leftResolvedType;
      else
        return createError(ERROR_TYPE::TYPE,
                           "incompatible types for operator /");
    }
    return createError(ERROR_TYPE::TYPE,
                       "operator / not applicable for given types: " +
                           leftResolvedType->toString() + " and " +
                           rightResolvedType->toString());
  }

  // Caso: operador % (módulo)
  if (operatorToken == TokenType::OPERATOR_MODULE) {
    auto leftNumeric = std::dynamic_pointer_cast<BasicType>(leftResolvedType);
    auto rightNumeric = std::dynamic_pointer_cast<BasicType>(rightResolvedType);
    if (leftNumeric && rightNumeric) {
      if ((leftNumeric->baseKind() == BasicKind::Int ||
           leftNumeric->baseKind() == BasicKind::Char) &&
          (rightNumeric->baseKind() == BasicKind::Int ||
           rightNumeric->baseKind() == BasicKind::Char)) {
        return leftResolvedType;
      } else {
        return createError(ERROR_TYPE::TYPE,
                           "operator % not applicable for non-integral types");
      }
    }
    return createError(ERROR_TYPE::TYPE,
                       "operator % not applicable for given types: " +
                           leftResolvedType->toString() + " and " +
                           rightResolvedType->toString());
  }

  // Caso: operadores lógicos && y ||
  if (operatorToken == TokenType::AND || operatorToken == TokenType::OR) {
    auto leftBool = std::dynamic_pointer_cast<BasicType>(leftResolvedType);
    auto rightBool = std::dynamic_pointer_cast<BasicType>(rightResolvedType);
    if (leftBool && rightBool && leftBool->baseKind() == BasicKind::Bool &&
        rightBool->baseKind() == BasicKind::Bool) {
      return boolType();
    }
    return createError(ERROR_TYPE::TYPE,
                       "logical operators require boolean operands: " +
                           leftResolvedType->toString() + " and " +
                           rightResolvedType->toString());
  }

  // Caso: operadores de igualdad (== y !=)
  if (operatorToken == TokenType::EQUAL ||
      operatorToken == TokenType::NOTEQUAL) {
    const bool leftIsNull =
        (std::dynamic_pointer_cast<NullType>(leftResolvedType) != nullptr);
    const bool rightIsNull =
        (std::dynamic_pointer_cast<NullType>(rightResolvedType) != nullptr);

    if (leftIsNull || rightIsNull) {
      // Verificar que el otro operando es efectivamente un puntero.
      if (leftIsNull &&
          !std::dynamic_pointer_cast<PointerType>(rightResolvedType))
        return createError(ERROR_TYPE::TYPE,
                           "cannot compare non-pointer type with null");
      if (rightIsNull &&
          !std::dynamic_pointer_cast<PointerType>(leftResolvedType))
        return createError(ERROR_TYPE::TYPE,
                           "cannot compare non-pointer type with null");

      return boolType();
    }

    // Si ninguno es NullType, procedemos con la comparación habitual:
    if (areSameType(leftResolvedType, rightResolvedType) ||
        haveCommonAncestor(leftResolvedType, rightResolvedType)) {
      return boolType();
    }

    return createError(ERROR_TYPE::TYPE,
                       "incompatible types for equality operator: " +
                           leftResolvedType->toString() + " vs " +
                           rightResolvedType->toString());
  }

  // Caso: operadores relacionales (<, >, <=, >=)
  if (operatorToken == TokenType::OPERATOR_SMALLER ||
      operatorToken == TokenType::OPERATOR_GREATER ||
      operatorToken == TokenType::SMALLEREQUAL ||
      operatorToken == TokenType::BIGGEREQUAL) {
    auto leftNumeric = std::dynamic_pointer_cast<BasicType>(leftResolvedType);
    auto rightNumeric = std::dynamic_pointer_cast<BasicType>(rightResolvedType);
    if (leftNumeric && rightNumeric) {
      if (areSameType(leftResolvedType, rightResolvedType) ||
          haveCommonAncestor(leftResolvedType, rightResolvedType)) {
        return boolType();
      } else {
        return createError(ERROR_TYPE::TYPE,
                           "incompatible types for relational operator: " +
                               leftResolvedType->toString() + " vs " +
                               rightResolvedType->toString());
      }
    }
    return createError(ERROR_TYPE::TYPE,
                       "relational operators not applicable for given types: " +
                           leftResolvedType->toString() + " and " +
                           rightResolvedType->toString());
  }

  // Aquí se podrían agregar casos para tipos definidos por el usuario y enums,
  // etc. Por ejemplo, si el operando izquierdo es de UserType, se intentaría
  // llamar a un método sobrecargado que implemente el operador.
  if (auto leftUserType =
          std::dynamic_pointer_cast<UserType>(leftResolvedType)) {
    std::string methodName;
    switch (operatorToken) {
    case TokenType::OPERATOR_ADD:
      methodName = "addable";
      break;
    case TokenType::OPERATOR_SUB:
      methodName = "substrable";
      break;
    case TokenType::OPERATOR_MULT:
      methodName = "multiplicable";
      break;
    case TokenType::OPERATOR_DIV:
      methodName = "divisble";
      break;
    case TokenType::AND:
      methodName = "andable";
      break;
    case TokenType::OR:
      methodName = "orable";
      break;
    case TokenType::EQUAL:
      methodName = "equal";
      break;
    case TokenType::NOTEQUAL:
      methodName = "notEqual";
      break;
    default:
      break;
    }
    if (!methodName.empty()) {
      std::expected<std::vector<Method>, Error> userMethodsExp =
          leftUserType->getMethods(methodName);
      if (!userMethodsExp)
        return createError(userMethodsExp.error());
      std::vector<Method> userMethods = userMethodsExp.value();
      for (const Method &userMethod : userMethods) {
        if (userMethod.params().size() == 1) {
          // Verificar compatibilidad del parámetro con rightResolvedType.
          if (canAssign(userMethod.params().params().front().second,
                        rightResolvedType))
            return userMethod.returnType();
        }
      }
      return createError(ERROR_TYPE::TYPE,
                         "user type " + leftUserType->name() +
                             " does not overload operator " +
                             tokenTypeToString(operatorToken));
    }
  }

  // Caso para EnumType: Se permiten operadores de igualdad y desigualdad.
  if (auto leftEnumType =
          std::dynamic_pointer_cast<EnumType>(leftResolvedType)) {
    if (auto rightEnumType =
            std::dynamic_pointer_cast<EnumType>(rightResolvedType)) {
      if (operatorToken == TokenType::EQUAL ||
          operatorToken == TokenType::NOTEQUAL) {
        return boolType();
      } else {
        return createError(ERROR_TYPE::TYPE,
                           "operator " + tokenTypeToString(operatorToken) +
                               " not allowed on enums");
      }
    }
    return createError(ERROR_TYPE::TYPE,
                       "incompatible types for binary operator: enum mismatch");
  }

  // Caso para PlaceHolder: no se permiten operaciones binarias sobre generics
  // sin resolver.
  if (std::dynamic_pointer_cast<PlaceHolder>(leftResolvedType) ||
      std::dynamic_pointer_cast<PlaceHolder>(rightResolvedType)) {
    return leftResolvedType;
  }

  // Si leftOperand es un PointerType y ningún otro caso aplicó:
  if (auto leftPointerType =
          std::dynamic_pointer_cast<PointerType>(leftResolvedType)) {
    if (operatorToken == TokenType::OPERATOR_SUB) {
      // Si ambos operandos son punteros, el resultado es un entero.
      if (std::dynamic_pointer_cast<PointerType>(rightResolvedType)) {
        return intType();
      }
    }
  }

  return createError(
      ERROR_TYPE::TYPE,
      "operator " + tokenTypeToString(operatorToken) +
          " not implemented for given types: " + leftResolvedType->toString() +
          " and " + rightResolvedType->toString());
}

std::expected<std::shared_ptr<Type>, Error> TypeTable::applyGenericReplacements(
    const std::shared_ptr<Type> &type,
    const std::vector<GenericParameter> &genericParams,
    const std::vector<std::shared_ptr<Type>> &replacements) const noexcept {
  if (!type) {
    return createError(ERROR_TYPE::TYPE, "null type in generic replacement");
  }

  // 1) Si es un placeholder, sustituimos directamente:
  if (auto ph = std::dynamic_pointer_cast<PlaceHolder>(type)) {
    // Buscamos el índice del parámetro genérico
    auto it =
        std::find_if(genericParams.begin(), genericParams.end(), [&](auto &gp) {
          return gp.name() == ph->getGenericParameter().name();
        });
    if (it == genericParams.end()) {
      return createError(ERROR_TYPE::TYPE,
                         "placeholder refers to unknown generic: " +
                             ph->getGenericParameter().name());
    }
    auto dist = std::distance(genericParams.begin(), it);
    size_t idx = static_cast<size_t>(dist);

    if (idx >= replacements.size()) {
      return createError(ERROR_TYPE::TYPE,
                         "missing replacement for generic " +
                             ph->getGenericParameter().name());
    }
    return replacements[idx];
  }

  // 2) Const, punteros, vectores: descendemos recursivamente
  if (auto ct = std::dynamic_pointer_cast<ConstType>(type)) {
    auto inner =
        applyGenericReplacements(ct->baseType(), genericParams, replacements);
    if (!inner)
      return inner;
    return std::make_shared<ConstType>(inner.value());
  }
  if (auto pt = std::dynamic_pointer_cast<PointerType>(type)) {
    auto inner =
        applyGenericReplacements(pt->baseType(), genericParams, replacements);
    if (!inner)
      return inner;
    return std::make_shared<PointerType>(inner.value());
  }
  if (auto vt = std::dynamic_pointer_cast<VectorType>(type)) {
    auto inner = applyGenericReplacements(vt->elementType(), genericParams,
                                          replacements);
    if (!inner)
      return inner;
    return std::make_shared<VectorType>(inner.value());
  }

  // 3) Instancias genéricas y user‑types: bajamos y propagamos
  if (auto git = std::dynamic_pointer_cast<GenericInstanceType>(type)) {
    for (size_t i = 0; i < git->typeArgs().size(); ++i) {
      auto sub = applyGenericReplacements(git->typeArgs()[i], genericParams,
                                          replacements);
      if (!sub)
        return sub;
      if (auto err = git->setGenericReplacement(i, sub.value()); !err)
        return createError(err.error());
    }
    return git;
  }
  if (auto ut = std::dynamic_pointer_cast<UserType>(type)) {
    // No es placeholder (ese ya lo cubrimos arriba), así que devolvemos tal
    // cual:
    return ut;
  }
  if (auto et = std::dynamic_pointer_cast<EnumType>(type)) {
    return et;
  }

  // 4) Básicos, void, null… sin cambios
  if (std::dynamic_pointer_cast<BasicType>(type) ||
      std::dynamic_pointer_cast<VoidType>(type) ||
      std::dynamic_pointer_cast<NullType>(type)) {
    return type;
  }

  return createError(ERROR_TYPE::TYPE,
                     "unexpected type form in generic replacement: " +
                         type->toString());
}

} // namespace nicole