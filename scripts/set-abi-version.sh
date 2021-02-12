#!/usr/bin/env bash
# 
# Copyright (C) 2020 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
SELF=${0##*/}

[ -n "$PATCHELF" ] || {
  echo "$SELF: patchelf command not defined (PATCHELF variable not set)"
  exit 1
}

ABI_VER="$1"
PATCH_DIR="$2"
REF_LIST="$3"

[ -n "$ABI_VER" -a -n "$PATCH_DIR" ] || {
	echo "$SELF: no ABI version or files/directories specified"
	echo "usage: $SELF <VERSION> [<PATH>...]"
	exit 1
}

cmd() {
	echo "$@" >&2
	"$@" || exit 1
}

gen_lib_list() {
	while read F; do
		F="${F##*/}"
		case "$F" in
			lib*.so*);;
			*) continue;;
		esac
		echo -n "$F:"
	done < "$REF_LIST"
}

find "$PATCH_DIR" -type f -a -name 'lib*.so*' | \
(
	while read F; do
		NEW_F="${F%%.so*}.so.$ABI_VER"
		NEW_NAME="${NEW_F##*/}"
		[ "$NEW_F" != "$F" ] || continue
		cmd mv "$F" "$NEW_F"
		[ "$REF_LIST" ] || cmd ln -s "$NEW_NAME" "$F"
		cmd $PATCHELF --set-soname "$NEW_NAME" "$NEW_F"
	done
)

[ -n "$REF_LIST" ] || exit 0

LIBS="$(gen_lib_list)"
LIBS="${LIBS%%:}"
find "$PATCH_DIR" -type f -a -exec file {} \; | \
  sed -n -e 's/^\(.*\):.*ELF.*\(executable\|relocatable\|shared object\).*,.*/\1:\2/p' | \
(
	IFS=":"
	while read F S; do
		$PATCHELF --print-needed "$F" | while read L; do
			BASE_L="${L%%.so*}"
			for lib in $LIBS; do
				base_lib="${lib%%.so*}"
				[ "$BASE_L" = "$base_lib" ] || continue
				[ "$l" = "$lib" ] && continue
				cmd $PATCHELF --replace-needed "$L" "$lib" "$F"
			done
		done
	done
	true
)
