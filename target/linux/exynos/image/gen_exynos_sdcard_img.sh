#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only

set -e
[ $# -eq 5 ] || {
    echo "SYNTAX: $0 <file> <bootfs image> <rootfs image> <bootfs size> <rootfs size>"
    exit 1
}

OUTPUT="$1"
BOOTFS="$2"
ROOTFS="$3"
BOOTFSSIZE="$4"
ROOTFSSIZE="$5"

# 4 MiB alignment leaves sectors 1..8191 free for BL1, BL2, U-Boot, TZSW and
# the U-Boot environment
head=4
sect=63

set $(ptgen -o "$OUTPUT" -h $head -s $sect -l 4096 -S 0x$SIGNATURE -a 1 -t 83 -p "${BOOTFSSIZE}M" -t 83 -p "${ROOTFSSIZE}M")

BOOTOFFSET="$(($1 / 512))"
ROOTFSOFFSET="$(($3 / 512))"

dd bs=512 if="$BOOTFS" of="$OUTPUT" seek="$BOOTOFFSET" conv=notrunc
dd bs=512 if="$ROOTFS" of="$OUTPUT" seek="$ROOTFSOFFSET" conv=notrunc
