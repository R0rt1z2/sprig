#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

INPUT="${1:-$SCRIPT_DIR/bin/lk.img}"
PARTITION="${2:-bl2_ext}"

python3 -m lkpatcher $INPUT -d $PARTITION

INPUT_BASE=$(basename "${INPUT%.img}")
mv "${INPUT_BASE}_${PARTITION}.bin" "${PARTITION}.bin"