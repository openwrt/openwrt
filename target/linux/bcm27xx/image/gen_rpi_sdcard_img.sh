#!/bin/sh

set -x
[ $# -eq 5 ] || {
    echo "SYNTAX: $0 <file> <bootfs image> <rootfs image> <bootfs size> <rootfs size>"
    exit 1
}

OUTPUT="$1"
BOOTFS="$2"
ROOTFS="$3"
BOOTFSSIZE="$4"
ROOTFSSIZE="$5"

head=4
sect=63

set $(ptgen -o $OUTPUT -h $head -s $sect -l 4096 -t c -p ${BOOTFSSIZE}M -t 83 -p ${ROOTFSSIZE}M)

BOOTOFFSET="$(($1 / 512))"
BOOTSIZE="$(($2 / 512))"
ROOTFSOFFSET="$(($3 / 512))"
ROOTFSSIZE="$(($4 / 512))"

dd bs=512 if="$BOOTFS" of="$OUTPUT" seek="$BOOTOFFSET" conv=notrunc
dd bs=512 if="$ROOTFS" of="$OUTPUT" seek="$ROOTFSOFFSET" conv=notrunc

# Add a bit to the end of the rootfs to ensure we destroy the overlay filesystem
# (which will cause it to be recreated). If we don't do this, any upgrade
# with the same ROOTFSIMGSIZE will retain the overlay - and hence all settings/packages -
# regardless of whether you keep settings.
# Note that ROOTFSSIZE is the _partition_ size, whereas ROOTFSIMGSIZE is the image
# we're writing. Any overlay is placed at the end of this image in the same partition.
# Taken from: https://github.com/openwrt/openwrt/issues/9113#issuecomment-1572846457

ROOTFSIMGBYTES="$(stat -c '%s' $ROOTFS)"
ROOTFSIMGSIZE="$(( ($ROOTFSIMGBYTES+511) / 512))"  # i.e. round up
OVERLAYOFFSET="$(($ROOTFSOFFSET + $ROOTFSIMGSIZE))"

# Calculate the number of remaining blocks in the partition after the rootfs image.
OVERLAYSIZE="$(($ROOTFSSIZE - ROOTFSIMGSIZE))"
PADSIZE="$OVERLAYSIZE"
# Just enough padding (1mb) to mess up any existing overlay if there's space.
if [ "$PADSIZE" -gt 2048 ]; then
  PADSIZE=2048
fi

# If PADSIZE=0, it almost certainly means there's no overlay.
# At any rate, nothing to do here.
if [ "$PADSIZE" -gt 0 ]; then
  echo "Padding $PADSIZE at $OVERLAYOFFSET"
  dd bs=512 if=/dev/zero of="$OUTPUT" seek="$OVERLAYOFFSET" count="$PADSIZE" conv=notrunc
fi


