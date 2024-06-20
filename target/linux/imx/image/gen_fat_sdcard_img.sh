#!/bin/sh
# Copyright (C) 2006-2012 OpenWrt.org
set -e -x
if [ $# -ne 5 ] && [ $# -ne 6 ]; then
    echo "SYNTAX: $0 <file> <kernel size> <kernel directory> <rootfs size> <rootfs image> [<align>]"
    exit 1
fi

OUTPUT="$1"
BOOTFSSIZE="$2"
BOOTFS="$3"
ROOTFSSIZE="$4"
ROOTFSIMAGE="$5"
ALIGN="${6:-1024}"

rm -f "$OUTPUT"

head=16
kernel_type=c
rootfs_type=83
sect=63

# create partition table
set $(ptgen -o "$OUTPUT" -h $head -s $sect  -l "$ALIGN" \
		-t "$kernel_type" -p "${BOOTFSSIZE}m" \
		-t "$rootfs_type" -p "${ROOTFSSIZE}m" )

BOOTFS_OFFSET="$(($1 / 512))"
ROOTFS_OFFSET="$(($3 / 512))"

dd if="$BOOTFS" of="$OUTPUT" bs=512 seek="$BOOTFS_OFFSET" conv=notrunc
dd if="$ROOTFSIMAGE" of="$OUTPUT" bs=512 seek="$ROOTFS_OFFSET" conv=notrunc
