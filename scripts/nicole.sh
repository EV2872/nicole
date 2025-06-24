#!/usr/bin/env bash
set -euo pipefail

# --- Detectar rutas ---
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
PROJECT_ROOT="${SCRIPT_DIR}/.."

# --- Ruta al ejecutable ---
BINARY="${PROJECT_ROOT}/build/bin/binaries"

if [ ! -x "${BINARY}" ]; then
  echo "❌ Ejecutable no encontrado: ${BINARY}" >&2
  echo "   Primero lanza: scripts/nicolecompile.sh" >&2
  exit 1
fi

# --- Ejecutar con todos los args pasados ---
exec "${BINARY}" "$@"
