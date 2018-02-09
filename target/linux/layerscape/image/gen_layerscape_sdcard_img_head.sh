#!/usr/bin/env bash

#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

set -x
[ $# -eq 3 ] || {
    echo "SYNTAX: $0 <file> <bootfs size> <rootfs size>"
    exit 1
}

OUTPUT="$1"
BOOTFSSIZE="$2"
ROOTFSSIZE="$3"

head=4
sect=63

set `ptgen -o $OUTPUT -h $head -s $sect -l 2048 -t c -p ${BOOTFSSIZE}M -t 83 -p ${ROOTFSSIZE}M`

