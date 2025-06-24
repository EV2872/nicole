#!/usr/bin/env bash
set -euo pipefail

# --- Detectar rutas ---
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

# --- Llamar al compilador ---
"${SCRIPT_DIR}/nicoleCompile.sh" "$@"

# Si pediste cobertura (-t), no seguimos con la ejecución
if [[ "${1:-}" == "-t" ]]; then
  exit 0
fi

# --- Ejecutar el binario ---
"${SCRIPT_DIR}/nicole.sh" "$@"
