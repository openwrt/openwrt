#!/bin/sh
# Append the MR6400 local policy additions to boarddetectsysagent.cil,
# substituting the module's actual top-level block name into the
# template. Appending to an existing module file (rather than adding a
# new file) guarantees the additions are compiled regardless of how the
# policy build collects its sources, and deriving the block name keeps
# this independent of upstream renames. CIL is declaration-order
# independent, so appending an (in ...) block is equivalent to patching
# the rules into the block itself.
set -e

BUILD_DIR="$1"
FILES_DIR="$(cd "$(dirname "$0")" && pwd)"
MOD="$BUILD_DIR/src/agent/sysagent/boarddetectsysagent.cil"

[ -f "$MOD" ] || {
	echo "mr6400-local-cil: $MOD does not exist" >&2
	exit 1
}

NS="$(sed -n 's/^(block \([A-Za-z0-9_]*\).*/\1/p' "$MOD" | head -n1)"
[ -n "$NS" ] || {
	echo "mr6400-local-cil: cannot determine block name in $MOD" >&2
	exit 1
}

{
	echo ""
	sed "s/@BOARDDETECT@/$NS/g" "$FILES_DIR/mr6400-local.cil.in"
} >> "$MOD"
