#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
#
# Assemble the boot initramfs for the UniFi Cloud Gateway Fiber.
#
# usage: mkinitrd.sh <target-rootfs-dir> <init> <output.cpio>
#
# The initramfs holds busybox from the target rootfs, the shared libraries it
# links against, and <init>. <init> finds the ext4 "root" partition, loop
# mounts the rootfs squashfs from it and switches to that.

set -e
umask 022

ROOT="$1"
INIT="$2"
OUT="$3"

[ -d "$ROOT" ] && [ -f "$INIT" ] && [ -n "$OUT" ] || {
	echo "usage: $0 <target-rootfs-dir> <init> <output.cpio>" >&2
	exit 1
}

READELF="${READELF:-readelf}"
CPIO="${CPIO:-cpio}"
APPLETS="cat cut echo grep ls mkdir mknod mount sed sh sha256sum sleep switch_root umount"

tmp="$(mktemp -d)"
chmod 0755 "$tmp"
trap 'rm -rf "$tmp"' EXIT

needed() {
	"$READELF" -d "$1" | sed -n 's/.*(NEEDED).*\[\(.*\)\]/\1/p'
}

find_lib() {
	local dir

	for dir in lib usr/lib; do
		[ -e "$ROOT/$dir/$1" ] && { echo "$ROOT/$dir/$1"; return 0; }
	done
	return 1
}

mkdir -p "$tmp/bin" "$tmp/dev" "$tmp/etc" "$tmp/lib" "$tmp/mnt/root" \
	"$tmp/mnt/sq" "$tmp/proc" "$tmp/sbin" "$tmp/sys" "$tmp/tmp"

cp "$ROOT/bin/busybox" "$tmp/bin/busybox"
for applet in $APPLETS; do
	ln -s busybox "$tmp/bin/$applet"
done
install -m 0755 "$INIT" "$tmp/init"

# The musl dynamic loader is libc.so itself; install it under the name
# busybox requests as its interpreter.
interp="$("$READELF" -l "$ROOT/bin/busybox" |
	sed -n 's|.*interpreter: \(/[^]]*\)\].*|\1|p')"
[ -n "$interp" ] || { echo "$0: no interpreter in busybox" >&2; exit 1; }
mkdir -p "$tmp$(dirname "$interp")"
cp -L "$ROOT$interp" "$tmp$interp"

# Every other library busybox needs, and what those need in turn.
seen=" "
queue="$ROOT/bin/busybox"
while [ -n "$queue" ]; do
	set -- $queue
	file="$1"
	shift
	queue="$*"
	for name in $(needed "$file"); do
		case "$seen" in
		*" $name "*) continue ;;
		esac
		seen="$seen$name "
		[ "$name" = libc.so ] && continue
		src="$(find_lib "$name")" || {
			echo "$0: $name not found in $ROOT" >&2
			exit 1
		}
		cp -L "$src" "$tmp/lib/$name"
		queue="$queue $src"
	done
done

[ -z "$SOURCE_DATE_EPOCH" ] ||
	find "$tmp" -exec touch -h -d "@$SOURCE_DATE_EPOCH" {} +

(cd "$tmp" && find . | LC_ALL=C sort |
	"$CPIO" --reproducible -o -H newc -R 0:0 --quiet) >"$OUT"
