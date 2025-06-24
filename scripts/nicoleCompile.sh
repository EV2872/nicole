#!/usr/bin/env bash
set -euo pipefail

# Detectar la carpeta donde vive este script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
# Suponiendo que scripts/ está justo dentro del root del proyecto:
PROJECT_ROOT="${SCRIPT_DIR}/.."

# Comprobar si Ninja está instalado y definir el generador
if command -v ninja &> /dev/null; then
    GENERATOR="-G Ninja"
else
    GENERATOR=""
fi

# Crear el directorio build si no existe
if [ ! -d "${PROJECT_ROOT}/build" ]; then
    mkdir -p "${PROJECT_ROOT}/build"
    pushd "${PROJECT_ROOT}/build" > /dev/null
    cmake ${GENERATOR} "${PROJECT_ROOT}"
    cmake --build . --parallel "$(nproc)"
    popd > /dev/null
fi

# Si se pasó la opción -t, regenerar con coverage
if [[ "${1:-}" == "-t" ]]; then
    pushd "${PROJECT_ROOT}/build" > /dev/null
    cmake ${GENERATOR} -DENABLE_COVERAGE=ON "${PROJECT_ROOT}"
    cmake --build . --parallel "$(nproc)"

    # Detectar compilador empleado
    if [ -f CMakeCache.txt ]; then
        COMPILER=$(grep -m1 '^CMAKE_CXX_COMPILER:' CMakeCache.txt | cut -d'=' -f2)
    else
        echo "CMakeCache.txt no encontrado, detectando compilador…"
        if command -v clang++ &> /dev/null; then
            COMPILER=clang++
        elif command -v g++ &> /dev/null; then
            COMPILER=g++
        else
            echo "No se encontró clang++ ni g++." >&2
            exit 1
        fi
    fi
    echo "Compilador utilizado: ${COMPILER}"

    if [[ "${COMPILER}" =~ clang ]]; then
        echo "Usando llvm-cov (Clang)…"
        export LLVM_PROFILE_FILE="coverage_%p.profraw"
        ctest --verbose
        llvm-profdata merge -sparse coverage_*.profraw -o coverage.profdata
        llvm-cov export ./bin/tests -instr-profile=coverage.profdata -format=lcov \
          > coverage.info
    else
        echo "Usando gcov (GCC)…"
        ctest --verbose
        lcov --capture --directory . --output-file coverage.info
    fi

    genhtml coverage.info --output-directory "${PROJECT_ROOT}/docs/"
    echo "Informe de cobertura en docs/."
    exit 0
fi

# Compilar sin cobertura
pushd "${PROJECT_ROOT}/build" > /dev/null
cmake --build . --parallel "$(nproc)"
popd > /dev/null