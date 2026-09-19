#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
set -eu

usage() {
	cat <<'EOH'
Usage: make_watchguard_t30w_bootfs.sh <host-bin-dir> <kernel> <dtb> <output.img>
EOH
}

[ $# -eq 4 ] || {
	usage >&2
	exit 1
}

hostbin="$1"
kernel="$2"
dtb="$3"
output_img="$4"
workdir="$(mktemp -d)"

cleanup() {
	rm -rf "$workdir"
}
trap cleanup EXIT INT TERM

[ -f "$kernel" ] || {
	echo "Missing kernel image: $kernel" >&2
	exit 1
}
[ -f "$dtb" ] || {
	echo "Missing DTB: $dtb" >&2
	exit 1
}
[ -x "$hostbin/mkfs.ext2" ] || {
	echo "Missing host mkfs.ext2: $hostbin/mkfs.ext2" >&2
	exit 1
}

mkdir -p "$workdir/boot"
cp "$kernel" "$workdir/boot/uImage_t30"
cp "$dtb" "$workdir/boot/t30.dtb"

used_kib=$(du -sk "$workdir/boot" | cut -f1)
blocks=$((used_kib + 8192))
[ "$blocks" -ge 32768 ] || blocks=32768
blocks=$((((blocks + 1023) / 1024) * 1024))

# Added -I 128 -O ^64bit for U-Boot 2010 compatibility
"$hostbin/mkfs.ext2" -q -F -I 128 -O ^64bit -d "$workdir/boot" "$output_img" "$blocks"
