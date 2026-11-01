#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
tmp=$(mktemp -d "${TMPDIR:-/tmp}/gen-dependencies.XXXXXX")
trap 'rm -rf "$tmp"' EXIT

printf '\0' > "$tmp/input"
printf 'depends=xxhash\0' > "$tmp/modinfo"
objcopy -I binary -O elf64-x86-64 -B i386 "$tmp/input" "$tmp/xxhash_generic.ko"
objcopy --add-section .modinfo="$tmp/modinfo" \
	--set-section-flags .modinfo=alloc,load,readonly,data \
	"$tmp/xxhash_generic.ko"

TMP_DIR="$tmp" "$SCRIPT_DIR/gen-dependencies.sh" "$tmp" > "$tmp/base"
grep -qxF xxhash.ko "$tmp/base"

printf 'kernel/lib/xxhash.ko\n' > "$tmp/modules.builtin"
TMP_DIR="$tmp" KERNEL_MODULES_BUILTIN="$tmp/modules.builtin" \
	"$SCRIPT_DIR/gen-dependencies.sh" "$tmp" > "$tmp/fixed"
! grep -q . "$tmp/fixed"
