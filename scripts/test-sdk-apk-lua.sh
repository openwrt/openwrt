#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Regression test for the SDK apk host-build dependency.
#
# Copyright (C) 2026 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

set -eu

SCRIPTDIR=$(dirname "$0")
BASEDIR=$(cd "$SCRIPTDIR/.." && pwd)
TEST_TMP=$(mktemp -d "${TMPDIR:-/tmp}/sdk-apk-lua.XXXXXX")
trap 'rm -rf "$TEST_TMP"' EXIT HUP INT TERM

cat > "$TEST_TMP/.packagedeps" <<'EOF'
package-m += system/apk
EOF

cat > "$TEST_TMP/Makefile" <<'EOF'
TOPDIR := @TOPDIR@
TMP_DIR := @TMP_DIR@
INCLUDE_DIR := $(TOPDIR)/include
SCRIPT_DIR := $(TOPDIR)/scripts
BUILD_DIR := $(TOPDIR)/build_dir
STAGING_DIR := $(TOPDIR)/staging_dir/target
CONFIG_USE_APK := y
SDK := 1
.DEFAULT_GOAL := dump

include $(TOPDIR)/rules.mk
curdir := package
include $(TOPDIR)/package/Makefile
include $(TOPDIR)/include/subdir.mk
$(eval $(call subdir,$(curdir)))

dump:
EOF

sed -i \
	-e "s|@TOPDIR@|$BASEDIR|" \
	-e "s|@TMP_DIR@|$TEST_TMP|" \
	"$TEST_TMP/Makefile"

make -rnp -f "$TEST_TMP/Makefile" > "$TEST_TMP/make.db"

APK_COMPILE='^package/system/apk/compile:.*package/system/apk/host/compile'
if ! grep -Eq "$APK_COMPILE" "$TEST_TMP/make.db"; then
	echo "SDK apk compile graph is missing its host build prerequisite" >&2
	exit 1
fi

echo "SDK apk lua host dependency test: ok"
