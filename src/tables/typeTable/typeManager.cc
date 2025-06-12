#include "../../../inc/tables/typeTable/typeManager.h"
#include <memory>

namespace nicole {

auto TypeManager::canAssign(const std::shared_ptr<Type> &dest,
                            const std::shared_ptr<Type> &src) const noexcept
    -> bool {
  // Caso 0: Si ambos punteros son iguales o alguno es nulo.
  if (dest == src) {
    return true;
  }
  if (!dest || !src) {
    return false;
  }

  // Caso 1: Manejo de tipos const (se "desenvuelven" para comparar el tipo
  // base).
  if (auto destConst = std::dynamic_pointer_cast<ConstType>(dest)) {
    return canAssign(destConst->baseType(), src);
  }
  if (auto srcConst = std::dynamic_pointer_cast<ConstType>(src)) {
    return canAssign(dest, srcConst->baseType());
  }

  // Caso 2: Tipos básicos.
  if (auto destBasic = std::dynamic_pointer_cast<BasicType>(dest)) {
    if (auto srcBasic = std::dynamic_pointer_cast<BasicType>(src)) {
      if (destBasic->baseKind() == srcBasic->baseKind()) {
        return true;
      }
      // Ejemplo de promoción: char -> int.
      if (srcBasic->baseKind() == BasicKind::Char &&
          destBasic->baseKind() == BasicKind::Int) {
        return true;
      }
      // Se pueden agregar otras reglas de conversión.
      return false;
    }
    return false;
  }

  // Caso 3: Tipo void.
  if (std::dynamic_pointer_cast<VoidType>(dest)) {
    return std::dynamic_pointer_cast<VoidType>(src) != nullptr;
  }

  // Caso 4: Tipo nulo.
  if (std::dynamic_pointer_cast<NullType>(src)) {
    // Se permite asignar nullptr a punteros o si ambos son nullptr.
    return std::dynamic_pointer_cast<PointerType>(dest) ||
           std::dynamic_pointer_cast<NullType>(dest);
  }

  // Caso 5: Tipos puntero.
  if (auto destPtr = std::dynamic_pointer_cast<PointerType>(dest)) {
    if (auto srcPtr = std::dynamic_pointer_cast<PointerType>(src)) {
      return canAssign(destPtr->baseType(), srcPtr->baseType());
    }
    return false;
  }

  // Caso 6: Tipos vector.
  if (auto destVec = std::dynamic_pointer_cast<VectorType>(dest)) {
    if (auto srcVec = std::dynamic_pointer_cast<VectorType>(src)) {
      return canAssign(destVec->elementType(), srcVec->elementType());
    }
    return false;
  }

  // Caso 7: Tipos de usuario y sus instancias genéricas.
  if (auto destUser = std::dynamic_pointer_cast<UserType>(dest)) {
    if (auto srcUser = std::dynamic_pointer_cast<UserType>(src)) {
      // Si se trata de instancias genéricas, se deben comparar los argumentos.
      if (auto destGen = std::dynamic_pointer_cast<GenericInstanceType>(dest)) {
        if (auto srcGen = std::dynamic_pointer_cast<GenericInstanceType>(src)) {
          if (destGen->name() != srcGen->name()) {
            return false;
          }
          const auto &destArgs = destGen->typeArgs();
          const auto &srcArgs = srcGen->typeArgs();
          if (destArgs.size() != srcArgs.size()) {
            return false;
          }
          for (size_t i = 0; i < destArgs.size(); ++i) {
            if (!canAssign(destArgs[i], srcArgs[i])) {
              return false;
            }
          }
          return true;
        }
        return false;
      }
      // Para tipos de usuario no genéricos, se permite si son iguales o si
      // 'dest' es un antecesor en la jerarquía de 'src'.
      if (destUser->name() == srcUser->name() ||
          destUser->isAboveInHearchy(srcUser)) {
        return true;
      }
      return false;
    }
    return false;
  }

  // Caso 8: Fallback a comparación de representaciones.
  return dest->toString() == src->toString();
}

} // namespace nicole