#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
#
# rustc wrapper for package builds. It passes options of unstable rustc to the
# target crates cargo compiles, and allows them for that crate alone: build
# scripts probe for unstable features, under the crate's own name, and change
# their code when they find them. A target crate of cargo carries --target,
# -C metadata and --error-format=json; probes and build scripts do not, and
# pass through unchanged.
#
# RUST_TARGET_WRAPPER_FLAGS      options for every target crate
# RUST_TARGET_WRAPPER_LTO_FLAGS  further options when the crate runs LTO
# RUST_TARGET_WRAPPER_NEXT       wrapper to run rustc with, such as sccache

rustc="$1"
shift

target=
metadata=
json=
lto=
crate=
prev=
for arg in "$@"; do
	case "$prev:$arg" in
	--target:*|*:--target=*) target=1 ;;
	-C:metadata=*|*:-Cmetadata=*) metadata=1 ;;
	*:--error-format=json) json=1 ;;
	--crate-name:*) crate="$arg" ;;
	-C:lto=off|-C:lto=no|-C:lto=false|*:-Clto=off|*:-Clto=no|*:-Clto=false) ;;
	-C:lto|-C:lto=*|*:-Clto|*:-Clto=*) lto=1 ;;
	esac
	prev="$arg"
done

if [ -z "$target" ] || [ -z "$metadata" ] || [ -z "$json" ] || [ -z "$crate" ]; then
	exec $RUST_TARGET_WRAPPER_NEXT "$rustc" "$@"
fi

exec env RUSTC_BOOTSTRAP="$crate" $RUST_TARGET_WRAPPER_NEXT "$rustc" "$@" \
	$RUST_TARGET_WRAPPER_FLAGS ${lto:+$RUST_TARGET_WRAPPER_LTO_FLAGS}
