#!/usr/bin/env bash
#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

set -ex
[ $# -eq 5 ] || {
    echo "SYNTAX: $0 <file> <preloader image> <uboot image> <kernel image> <rootfs system>"
    exit 1
}

OUTPUT_FILE="$1"
PRELOADER_FILE="$2"
UBOOT_FILE="$3"
KERNEL_FILE="$4"
ROOTFS_FILE="$5"

BS=1024
PRELOADER_OFFSET=0    # 0KB
UBOOT_OFFSET=320      # 320KB
KERNEL_OFFSET=2048    # 2048KB
ROOTFS_OFFSET=67584   # 67584KB

dd bs="$BS" if="$PRELOADER_FILE"      of="$OUTPUT_FILE"    seek="$PRELOADER_OFFSET" 
dd bs="$BS" if="$UBOOT_FILE"          of="$OUTPUT_FILE"    seek="$UBOOT_OFFSET" 
dd bs="$BS" if="$KERNEL_FILE"         of="$OUTPUT_FILE"    seek="$KERNEL_OFFSET" 
dd bs="$BS" if="$ROOTFS_FILE"         of="$OUTPUT_FILE"    seek="$ROOTFS_OFFSET" 
