#!/bin/sh
#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
SELF=${0##*/}

READELF="${READELF:-readelf}"
OBJCOPY="${OBJCOPY:-objcopy}"
TARGETS=$*
XARGS="${XARGS:-xargs -r}"
BUILTIN_MODULES="${KERNEL_MODULES_BUILTIN:-}"

[ -z "$TARGETS" ] && {
  echo "$SELF: no directories / files specified"
  echo "usage: $SELF [PATH...]"
  exit 1
}

find $TARGETS -type f -a -exec file {} \; | \
  sed -n -e 's/^\(.*\):.*ELF.*\(executable\|shared object\).*,.*/\1/p' | \
  $XARGS -n1 $READELF -d | \
  awk '$2 ~ /NEEDED/ && $NF !~ /interpreter/ && $NF ~ /^\[?lib.*\.so/ { gsub(/[\[\]]/, "", $NF); print $NF }' | \
  sort -u

tmp=$(mktemp $TMP_DIR/dep.XXXXXXXX)
if [ -n "$BUILTIN_MODULES" ] && [ -f "$BUILTIN_MODULES" ]; then
	builtin=$(mktemp $TMP_DIR/builtin.XXXXXXXX)
	sed -e 's,.*/,,' "$BUILTIN_MODULES" > "$builtin"
else
	builtin=
fi
for kmod in $(find $TARGETS -type f -name \*.ko); do
	$OBJCOPY -O binary -j .modinfo $kmod $tmp
	sed -e 's,\x00,\n,g' $tmp | \
		sed -ne '/^depends=.\+/ { s/^depends=//; s/,/.ko\n/g; s/$/.ko/p; q }'
done | sort -u | while read dep; do
	[ -n "$builtin" ] && grep -qxF "$dep" "$builtin" && continue
	echo "$dep"
done
rm -f $tmp $builtin
