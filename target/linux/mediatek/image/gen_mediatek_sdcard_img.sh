#!/usr/bin/env bash
#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

set -e
[ $# -eq 7 ] || {
    echo "SYNTAX: $0 <file> <preloader image> <u-boot image> <bootfs image> <rootfs image> <bootfs size> <rootfs size>"
    exit 1
}

OUTPUT="$1"
PRELOADER="$2"
UBOOT="$3"
BOOTFS="$4"
ROOTFS="$5"
BOOTFSSIZE="$6"
ROOTFSSIZE="$7"

head=4
sect=63

set `ptgen -o $OUTPUT -h $head -s $sect -l 1024 -t c -p ${BOOTFSSIZE}M -t 83 -p ${ROOTFSSIZE}M -a 0`

BOOT_OFFSET="$(($1 / 512))"
BOOT_SIZE="$(($2 / 512))"
ROOTFS_OFFSET="$(($3 / 512))"
ROOTFS_SIZE="$(($4 / 512))"

PRELOADER_OFFSET=2     # 2KB
UBOOT_OFFSET=320       # 320KB

SDMMC_BOOT="SDMMC_BOOT\x00\x00\x01\x00\x00\x00\x00\x02\x00\x00"
BRLYT="\
BRLYT\x00\x00\x00\x01\x00\x00\x00\x00\x08\x00\x00\
\x00\x08\x00\x00\x42\x42\x42\x42\x08\x00\x01\x00\x00\x08\x00\x00\
\x00\x08\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

echo -en "${SDMMC_BOOT}" | dd bs=1 of="${OUTPUT}" seek=0   conv=notrunc
echo -en "${BRLYT}"      | dd bs=1 of="${OUTPUT}" seek=512 conv=notrunc

dd bs=1024 if="${PRELOADER}" of="${OUTPUT}" seek="${PRELOADER_OFFSET}" conv=notrunc
dd bs=1024 if="${UBOOT}"     of="${OUTPUT}" seek="${UBOOT_OFFSET}"     conv=notrunc
dd bs=512  if="${BOOTFS}"    of="${OUTPUT}" seek="${BOOT_OFFSET}"      conv=notrunc
dd bs=512  if="${ROOTFS}"    of="${OUTPUT}" seek="${ROOTFS_OFFSET}"    conv=notrunc
