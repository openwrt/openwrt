#!/bin/bash

SCRIPTDIR="$(dirname "$0")"
[ "${SCRIPTDIR:0:1}" = "/" ] || SCRIPTDIR="$PWD/$SCRIPTDIR"
BASEDIR="$SCRIPTDIR/.."

DIR="$BASEDIR/tmp/deptest"
STAMP_DIR_SUCCESS="$DIR/stamp-success"
STAMP_DIR_FAILED="$DIR/stamp-failed"
BUILD_DIR="$DIR/build_dir/target"
BUILD_DIR_HOST="$DIR/build_dir/host"
STAGING_DIR="$DIR/staging_dir"
STAGING_DIR_HOST="$DIR/staging_dir_host"
STAGING_DIR_HOST_TMPL="$DIR/staging_dir_host_tmpl"
LOG_DIR="$DIR/logs"

die()
{
	echo "$@"
	exit 1
}

[ -f "$BASEDIR/include/toplevel.mk" ] || \
	die "Error: Could not find buildsystem base directory"
[ -f "$BASEDIR/.config" ] || \
	die "The buildsystem is not configured. Please run make menuconfig."
cd "$BASEDIR" || die "Failed to enter base directory"

mkdir -p "$STAMP_DIR_SUCCESS" "$STAMP_DIR_FAILED" "$BUILD_DIR" "$BUILD_DIR_HOST" "$LOG_DIR"

[ -d "$STAGING_DIR_HOST_TMPL" ] || {
	rm -rf staging_dir/host
	make tools/install V=99 || die "make tools/install failed, please check"
	cp -al staging_dir/host "$STAGING_DIR_HOST_TMPL"
	make toolchain/install V=99 || die "make toolchain/install failed, please check"
	make target/linux/install V=99 || die "make target/linux/install failed, please check"
}

for pkg in `cat tmp/.packagedeps  | grep CONFIG_PACKAGE | grep -v curdir | sed -e 's,.*[/=]\s*,,' | sort -u`; do
	SELECTED=
	for conf in `grep CONFIG_PACKAGE tmp/.packagedeps | grep -E "[ /]$pkg\$" | sed -e 's,package-$(\(CONFIG_PACKAGE_.*\)).*,\1,'`; do
		grep "$conf=" .config > /dev/null && SELECTED=1
	done
	STAMP_SUCCESS="$STAMP_DIR_SUCCESS/$pkg"
	STAMP_FAILED="$STAMP_DIR_FAILED/$pkg"
	rm -f "$STAMP_FAILED"
	[ -f "$STAMP_SUCCESS" ] && continue
	[ -n "$SELECTED" ] || {
		echo "Package $pkg is not selected"
		continue
	}
	echo "Testing package $pkg..."
	rm -rf "$STAGING_DIR"
	mkdir -p "$STAGING_DIR"
	rm -rf "$STAGING_DIR_HOST"
	cp -al "$STAGING_DIR_HOST_TMPL" "$STAGING_DIR_HOST"
	make package/$pkg/compile \
		BUILD_DIR="$BUILD_DIR" \
		BUILD_DIR_HOST="$BUILD_DIR_HOST" \
		STAGING_DIR="$STAGING_DIR" \
		STAGING_DIR_HOST="$STAGING_DIR_HOST" \
		V=99 >"$LOG_DIR/$(basename $pkg).log" 2>&1
	if [ $? -eq 0 ]; then
		touch "$STAMP_SUCCESS"
	else
		touch "$STAMP_FAILED"
		echo "Building package $pkg failed!"
	fi
done
