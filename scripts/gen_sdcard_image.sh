#!/usr/bin/env bash
set -x

[ $# -ge 5 ] || {
    echo "SYNTAX: $0 <file> <bootfs image> <rootfs image> <bootfs size> <rootfs size> <u-boot image>"
    exit 1
}

OUTPUT="$1"
BOOTFS="$2"
ROOTFS="$3"
BOOTFSSIZE="$4"
ROOTFSSIZE="$5"
UBOOT="$6"

[ -z "$SOURCE_DATE_EPOCH" ] || \
	SIGNATURE="-S 0x$(printf "%.8s" $SOURCE_DATE_EPOCH)"

head=4
sect=63

set `ptgen -o $OUTPUT -h $head -s $sect -l 1024 -t c -p ${BOOTFSSIZE}M -t 83 -p ${ROOTFSSIZE}M $SIGNATURE`

BOOTOFFSET="$(($1 / 512))"
BOOTSIZE="$(($2 / 512))"
ROOTFSOFFSET="$(($3 / 512))"
ROOTFSSIZE="$(($4 / 512))"

[ -n "$UBOOT" ] && \
	dd bs=1024 if="$UBOOT" of="$OUTPUT" seek=8 conv=notrunc

dd bs=512 if="$BOOTFS" of="$OUTPUT" seek="$BOOTOFFSET" conv=notrunc
dd bs=512 if="$ROOTFS" of="$OUTPUT" seek="$ROOTFSOFFSET" conv=notrunc
