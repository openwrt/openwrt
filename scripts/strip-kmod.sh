#!/usr/bin/env bash
[ -n "$CROSS" ] || {
	echo "The variable CROSS must be set to point to the cross-compiler prefix"
	exit 1
}

MODULE="$1"

[ "$#" -ne 1 ] && {
	echo "Usage: $0 <module>"
	exit 1
}

${CROSS}objcopy \
	--strip-unneeded \
	-R .comment \
	-R .pdr \
	-R .mdebug.abi32 \
	-R .note.gnu.build-id \
	-R .gnu.attributes \
	-R .reginfo \
	-G __this_module \
	-x "$MODULE" "$MODULE.tmp"

${CROSS}nm "$MODULE.tmp" | awk '
BEGIN {
	n = 0
}

$3 && $2 ~ /[brtd]/ && $3 !~ /\$LC/ {
	print "--redefine-sym "$3"=_"n;
	n = n + 1
}
' > "$MODULE.tmp1"

${CROSS}objcopy `cat ${MODULE}.tmp1` ${MODULE}.tmp ${MODULE}.out
mv "${MODULE}.out" "${MODULE}"
rm -f "${MODULE}".t*
