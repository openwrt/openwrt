#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
#
# C and C++ compiler for cc-rs when the target triple is the build host's:
# cc-rs then takes the host compiler for target code as well. Cargo runs the
# build scripts of target units with OUT_DIR in
# <target-dir>/<triple>/<profile>/build/<unit>/out.

kind=target
if [ -n "$OUT_DIR" ]; then
	triple_dir="${OUT_DIR%/*/*/*/*}"
	[ "${triple_dir##*/}" = "$TARGET" ] || kind=host
fi

case "$kind:${0##*/}" in
target:*cxx*) exec $TARGET_CXX $TARGET_CXXFLAGS "$@" ;;
target:*) exec $TARGET_CC $TARGET_CFLAGS "$@" ;;
host:*cxx*) exec $HOST_CXX "$@" ;;
*) exec $HOST_CC "$@" ;;
esac
