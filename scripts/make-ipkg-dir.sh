#!/bin/sh
BASE=http://svn.openwrt.org/openwrt/trunk/openwrt
TARGET=$1
CONTROL=$2
VERSION=$3
ARCH=$4

WD=$(pwd)

mkdir -p "$TARGET/CONTROL"
grep -v -e '^Version:' -e '^Architecture:' "$CONTROL" > "$TARGET/CONTROL/control"
grep '^Maintainer' "$CONTROL" >/dev/null 2>&1 || \
        echo "Maintainer: LEDE Community <lede-dev@lists.infradead.org>" >> "$TARGET/CONTROL/control"
grep '^Source' "$CONTROL" >/dev/null 2>&1 || {
        pkgbase=$(echo "$WD" | sed -e "s|^$TOPDIR/||g")
        [ "$pkgbase" = "$WD" ] && src="N/A" || src="$BASE/$pkgbase"
        echo "Source: $src" >> "$TARGET/CONTROL/control"
}
echo "Version: $VERSION" >> "$TARGET/CONTROL/control"
echo "Architecture: $ARCH" >> "$TARGET/CONTROL/control"
chmod 644 "$TARGET/CONTROL/control"
