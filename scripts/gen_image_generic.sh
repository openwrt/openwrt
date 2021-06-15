#!/bin/sh
# Copyright (C) 2006-2012 OpenWrt.org
set -e -x
if [ $# -ne 5 ] && [ $# -ne 6 ]; then
    echo "SYNTAX: $0 <file> <kernel size> <kernel directory> <rootfs size> <rootfs image> [<align>]"
    exit 1
fi

OUTPUT="$1"
KERNELSIZE="$2"
KERNELDIR="$3"
ROOTFSSIZE="$4"
ROOTFSIMAGE="$5"
ALIGN="$6"

rm -f "$OUTPUT"

head=16
sect=63

# create partition table
set $(ptgen -o "$OUTPUT" -h $head -s $sect \
      ${GUID:+-g -t ef -p 1m} \
      ${GUID:+-r -N "openwrt_boot"       -t ea} -p "${KERNELSIZE}m" \
      ${GUID:+-r -N "openwrt_rootfs"     -t 83} -p "${ROOTFSSIZE}m" \
      ${GUID:+-r -N "openwrt_rootfs_alt" -t 83  -p "${ROOTFSSIZE}m"} \
      ${ALIGN:+-l $ALIGN} ${SIGNATURE:+-S 0x$SIGNATURE} ${GUID:+-G $GUID})

if [ -n "$GUID" ]; then
    KERNELOFFSET="$(($3 / 512))"
    KERNELSIZE="$4"
    ROOTFSOFFSET="$(($5 / 512))"
    ROOTFSSIZE="$(($6 / 512))"
else
    KERNELOFFSET="$(($1 / 512))"
    KERNELSIZE="$2"
    ROOTFSOFFSET="$(($3 / 512))"
    ROOTFSSIZE="$(($4 / 512))"
fi

[ -n "$PADDING" ] && dd if=/dev/zero of="$OUTPUT" bs=512 seek="$ROOTFSOFFSET" conv=notrunc count="$ROOTFSSIZE"
[ -n "$GUID" ] && dd if="$OUTPUT.grub.img" of="$OUTPUT" bs=512 seek=512 conv=notrunc count=2048
dd if="$ROOTFSIMAGE" of="$OUTPUT" bs=512 seek="$ROOTFSOFFSET" conv=notrunc

if [ -n "$GUID" ]; then
    make_ext4fs -J -L openwrt_boot -l "$KERNELSIZE" "$OUTPUT.kernel" "$KERNELDIR"
else
    make_ext4fs -J -L kernel -l "$KERNELSIZE" "$OUTPUT.kernel" "$KERNELDIR"
fi
dd if="$OUTPUT.kernel" of="$OUTPUT" bs=512 seek="$KERNELOFFSET" conv=notrunc
rm -f "$OUTPUT.kernel"
