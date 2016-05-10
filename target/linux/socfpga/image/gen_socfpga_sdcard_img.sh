#!/usr/bin/env bash

#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

set -x 
[ $# -eq 5 ] || {
    echo "SYNTAX: $0 <file> <rootfs image> <rootfs size> <u-boot image> <cfg image>"
    exit 1
}

OUTPUT="$1"
ROOTFS="$2"
ROOTFSSIZE="$3"
UBOOT="$4"
CFGFS="$5"

head=4
sect=63

set `ptgen -o $OUTPUT -h $head -s $sect -l 1024	\
	-t a2 -p 1M				\
	-t 83 -p ${ROOTFSSIZE}M			\
	-t 83 -p 1M`

UBOOTOFFSET="$(($1 / 512))"
UBOOTSIZE="$(($2 / 512))"
ROOTFSOFFSET="$(($3 / 512))"
ROOTFSSIZE="$(($4 / 512))"
CFGFSOFFSET="$(($5 / 512))"
CFGFSSIZE="$(($6 / 512))"

dd bs=512 if="$UBOOT" of="$OUTPUT" seek="$UBOOTOFFSET" conv=notrunc
dd bs=512 if="$ROOTFS" of="$OUTPUT" seek="$ROOTFSOFFSET" conv=notrunc

mkdosfs "$CFGFS" -C 1024
dd bs=512 if="$CFGFS" of="$OUTPUT" seek="$CFGFSOFFSET" conv=notrunc
