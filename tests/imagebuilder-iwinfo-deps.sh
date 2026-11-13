#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only

set -eu

TOPDIR=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)
IWINFO_MAKEFILE="$TOPDIR/package/network/utils/iwinfo/Makefile"
WIFI_SCRIPTS_MAKEFILE="$TOPDIR/package/network/config/wifi-scripts/Makefile"

iwinfo_package=$(sed -n '/^define Package\/iwinfo$/,/^endef$/p' "$IWINFO_MAKEFILE")
wifi_scripts_package=$(sed -n \
	'/^define Package\/wifi-scripts$/,/^endef$/p' "$WIFI_SCRIPTS_MAKEFILE")

case "$iwinfo_package" in
	*'+WIFI_SCRIPTS_UCODE:wifi-scripts'*)
		;;
	*)
		echo "iwinfo does not pull wifi-scripts for ucode builds" >&2
		exit 1
		;;
esac

case "$wifi_scripts_package" in
	*'PKGARCH:=all'*)
		;;
	*)
		echo "wifi-scripts is not an ImageBuilder-installable package" >&2
		exit 1
		;;
esac

test -f "$TOPDIR/package/network/config/wifi-scripts/files-ucode/usr/bin/iwinfo"

echo "iwinfo -> wifi-scripts -> /usr/bin/iwinfo closure is present"
