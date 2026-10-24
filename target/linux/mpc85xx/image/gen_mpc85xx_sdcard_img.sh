#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Based on gen_mvebu_sdcard_img.sh, Copyright (C) 2016 Josua Mayer
#
# Usage: gen_mpc85xx_sdcard_img.sh <outfile> <type1> <size1> <image1> \
#	<type2> <size2> <image2>
#
# Write an MBR image with two partitions of the given types and sizes in
# MiB. Each image is followed by 128 KiB of zeros, so that no old overlay
# survives. The partitions start on 4 MiB boundaries, which leaves room
# below the first one for the raw mainline U-Boot SD image.

set -e

[ $# -eq 7 ] || {
	echo "usage: $0 <outfile> <type1> <size1> <image1> <type2> <size2> <image2>" >&2
	exit 1
}
[ -f "$4" ] && [ -f "$7" ] || {
	echo "$0: $4 or $7 is missing" >&2
	exit 1
}

OUTFILE="$1"
dd if=/dev/zero of="$OUTFILE" bs=512 count=1 2>/dev/null
parts=$(ptgen -o "$OUTFILE" -h 16 -s 63 -l 4096 -S "0x$SIGNATURE" \
	-t "$2" -p $(($3 * 1024 + 256)) -t "$5" -p $(($6 * 1024 + 256)))

# ptgen prints the offset and the size of each partition in bytes
set -- $parts "$4" "$7"
[ $# -eq 6 ] || {
	echo "$0: unexpected ptgen output: $parts" >&2
	exit 1
}

write_part() {
	dd if="$1" of="$OUTFILE" bs=512 seek=$(($2 / 512)) conv=notrunc 2>/dev/null
	dd if=/dev/zero of="$OUTFILE" bs=512 count=256 conv=notrunc 2>/dev/null \
		seek=$((($2 + $(wc -c < "$1") + 511) / 512))
}

write_part "$5" "$1"
write_part "$6" "$3"
