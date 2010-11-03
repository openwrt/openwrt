#!/bin/bash
#
# Automated OpenWrt package dependency checker
#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

SCRIPTDIR="$(dirname "$0")"
[ "${SCRIPTDIR:0:1}" = "/" ] || SCRIPTDIR="$PWD/$SCRIPTDIR"
BASEDIR="$SCRIPTDIR/.."

DIR="$BASEDIR/tmp/deptest"
STAMP_DIR_SUCCESS="$DIR/stamp-success"
STAMP_DIR_FAILED="$DIR/stamp-failed"
STAMP_DIR_BLACKLIST="$DIR/stamp-blacklist"
BUILD_DIR="$DIR/build_dir/target"
BUILD_DIR_HOST="$DIR/build_dir/host"
STAGING_DIR="$DIR/staging_dir"
STAGING_DIR_HOST="$STAGING_DIR/host"
STAGING_DIR_HOST_TMPL="$DIR/staging_dir_host_tmpl"
LOG_DIR="$DIR/logs"

die()
{
	echo "$@"
	exit 1
}

usage()
{
	echo "deptest.sh [OPTIONS] [PACKAGES]"
	echo
	echo "OPTIONS:"
	echo "  --lean       Run a lean test. Do not clean the build directory for each"
	echo "               package test."
	echo
	echo "PACKAGES are packages to test. If not specified, all installed packages"
	echo "will be tested."
}

test_package() # $1=pkgname
{
	local pkg="$1"
	[ -n "$pkg" -a -z "$(echo "$pkg" | grep -e '/')" -a "$pkg" != "." -a "$pkg" != ".." ] || \
		die "Package name \"$pkg\" contains illegal characters"
	local SELECTED=
	for conf in `grep CONFIG_PACKAGE tmp/.packagedeps | grep -E "[ /]$pkg\$" | sed -e 's,package-$(\(CONFIG_PACKAGE_.*\)).*,\1,'`; do
		grep "$conf=" .config > /dev/null && SELECTED=1 && break
	done
	local STAMP_SUCCESS="$STAMP_DIR_SUCCESS/$pkg"
	local STAMP_FAILED="$STAMP_DIR_FAILED/$pkg"
	local STAMP_BLACKLIST="$STAMP_DIR_BLACKLIST/$pkg"
	rm -f "$STAMP_FAILED"
	[ -f "$STAMP_SUCCESS" ] && return
	[ -n "$SELECTED" ] || {
		echo "Package $pkg is not selected"
		return
	}
	[ -f "$STAMP_BLACKLIST" ] && {
		echo "Package $pkg is blacklisted"
		return
	}
	echo "Testing package $pkg..."
	rm -rf "$STAGING_DIR"
	mkdir -p "$STAGING_DIR"
	cp -al "$STAGING_DIR_HOST_TMPL" "$STAGING_DIR_HOST"
	[ $lean_test -eq 0 ] && rm -rf "$BUILD_DIR" "$BUILD_DIR_HOST"
	mkdir -p "$BUILD_DIR" "$BUILD_DIR_HOST"
	make package/$pkg/compile \
		BUILD_DIR="$BUILD_DIR" \
		BUILD_DIR_HOST="$BUILD_DIR_HOST" \
		STAGING_DIR="$STAGING_DIR" \
		STAGING_DIR_HOST="$STAGING_DIR_HOST" \
		FORCE_HOST_INSTALL=1 \
		V=99 >"$LOG_DIR/$(basename $pkg).log" 2>&1
	if [ $? -eq 0 ]; then
		touch "$STAMP_SUCCESS"
	else
		touch "$STAMP_FAILED"
		echo "Building package $pkg failed!"
	fi
}

# parse commandline options
packages=
lean_test=0
while [ $# -ne 0 ]; do
	case "$1" in
	--help|-h)
		usage
		exit 0
		;;
	--lean)
		lean_test=1
		;;
	*)
		packages="$packages $1"
		;;
	esac
	shift
done

[ -f "$BASEDIR/include/toplevel.mk" ] || \
	die "Error: Could not find buildsystem base directory"
[ -f "$BASEDIR/.config" ] || \
	die "The buildsystem is not configured. Please run make menuconfig."
cd "$BASEDIR" || die "Failed to enter base directory"

mkdir -p "$STAMP_DIR_SUCCESS" "$STAMP_DIR_FAILED" "$STAMP_DIR_BLACKLIST" "$LOG_DIR"

[ -d "$STAGING_DIR_HOST_TMPL" ] || {
	rm -rf staging_dir/host
	make tools/install V=99 || die "make tools/install failed, please check"
	cp -al staging_dir/host "$STAGING_DIR_HOST_TMPL"
	make toolchain/install V=99 || die "make toolchain/install failed, please check"
	make target/linux/install V=99 || die "make target/linux/install failed, please check"
}

if [ -z "$packages" ]; then
	# iterate over all packages
	for pkg in `cat tmp/.packagedeps  | grep CONFIG_PACKAGE | grep -v curdir | sed -e 's,.*[/=]\s*,,' | sort -u`; do
		test_package "$pkg"
	done
else
	# only check the specified packages
	for pkg in $packages; do
		test_package "$pkg"
	done
fi
