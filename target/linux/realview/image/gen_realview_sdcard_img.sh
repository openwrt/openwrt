#!/usr/bin/env bash

set -x
[ $# -eq 3 ] || {
    echo "SYNTAX: $0 <file> <rootfs image> <rootfs size>"
    exit 1
}

OUTPUT="$1"
ROOTFS="$2"
ROOTFSSIZE="$3"

head=4
sect=63

set `ptgen -o $OUTPUT -h $head -s $sect -l 4096 -t 83 -p ${ROOTFSSIZE}M`

ROOTFSOFFSET="$(($1 / 512))"
ROOTFSSIZE="$(($2 / 512))"

dd bs=512 if="$ROOTFS" of="$OUTPUT" seek="$ROOTFSOFFSET" conv=notrunc


