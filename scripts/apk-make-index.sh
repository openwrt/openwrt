#!/usr/bin/env bash
set -e

pkg_dir=$1

if [ -z "$pkg_dir" ] || [ ! -d "$pkg_dir" ]; then
	echo "Usage: apk-make-index <package_directory>" >&2
	exit 1
fi

(
	cd "$pkg_dir" || exit 1
	GLOBIGNORE="kernel*:libc*"
	set -- *.apk
	if [ "$1" = '*.apk' ]; then
		echo "No APK packages found"
	fi
	apk index --output APKINDEX.tar.gz "$@"
	unset GLOBIGNORE
)
