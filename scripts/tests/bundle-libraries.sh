#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-2.0-only

set -euo pipefail

TOPDIR="$(CDPATH= cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)"
TESTDIR="$(mktemp -d)"
trap 'rm -rf "$TESTDIR"' EXIT

mkdir -p "$TESTDIR/bin"
cp "$(command -v readlink)" "$TESTDIR/bin/greadlink"
ln -s greadlink "$TESTDIR/bin/readlink"

PATH="/usr/bin:/bin" "$TOPDIR/scripts/bundle-libraries.sh" \
	"$TESTDIR" "$TESTDIR/bin/greadlink" >/dev/null

test "$(readlink "$TESTDIR/bin/readlink")" = .greadlink.bin
timeout 2 "$TESTDIR/bin/readlink" /proc/self/exe >/dev/null
