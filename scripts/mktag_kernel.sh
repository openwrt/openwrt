#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
set -e

usage() {
	printf "Usage: %s -s <rootfsLen> -o <outputfile>\n" "${0##*/}" >&2
	exit 1
}

rootfs_len=""
outfile=""

while getopts "s:o:h" opt; do
	case "$opt" in
		s) rootfs_len="$OPTARG" ;;
		o) outfile="$OPTARG" ;;
		h|*) usage ;;
	esac

done

[ -n "$rootfs_len" ] || usage
[ -n "$outfile" ] || usage

python3 - <<"PY" "$rootfs_len" "$outfile"
import struct
import sys

rootfs_len = int(sys.argv[1], 0)
outfile = sys.argv[2]

tag = struct.pack("<II", rootfs_len, 0) + (b"\x00" * (126 * 4))
with open(outfile, "wb") as f:
    f.write(tag)
PY
