#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Copyright (C) 2016 Josua Mayer

usage() {
	echo "$0 <outfile> [<bootloader> <bootloader-offset>] [<type_partitionN> <size_partitionN> <img_partitionN>]?"
}

# required arguments are complex, essentially all but one are optional:
# outfile always required
# 2 arguments for bootloader are specified together or skipped.
# Then up to 4 triples for partitions are allowed.
if [ $# -lt 1 ] || [ $# -gt 15 ] || [ $(($# % 3)) -eq 1 ]; then
	usage
	exit 1
fi

# always have first argument
OUTFILE="$1"

# how to know if bootloader arguments are given or skipped:
# with bootloader: total number of arguments multiple of 3
# without bootloader: total arguments multiple of 3, + 1
if [ $(($# % 3)) -eq 0 ]; then
	BOOTLOADER="$2"
	BOOTLOADER_OFFSET=$3
	shift 2
fi
shift 1

set -e

# generate image file
printf "Creating %s from /dev/zero: " "$OUTFILE"
dd if=/dev/zero of="$OUTFILE" bs=512 count=1 >/dev/null
printf "Done\n"

while [ "$#" -ge 3 ]; do
	ptgen_args="$ptgen_args -t $1 -p $(($2 * 1024 + 256))"
	parts="$parts$3 "
	shift; shift; shift
done

head=16
sect=63

# create real partition table using fdisk
printf "Creating partition table: "
set $(ptgen -o "$OUTFILE" -h $head -s $sect -l 8192 -S 0x$SIGNATURE $ptgen_args)
printf "Done\n"

# install bootloader
if [ -n "$BOOTLOADER" ]; then
	printf "Writing bootloader: "
	dd of="$OUTFILE" if="$BOOTLOADER" bs=512 seek=$BOOTLOADER_OFFSET conv=notrunc 2>/dev/null
	printf "Done\n"
fi

i=1
while [ "$#" -ge 2 ]; do
	img="${parts%% *}"
	parts="${parts#* }"

	printf "Writing %s to partition %i: " "$img" $i
	(
		cat "$img"
		# add padding to avoid leaving behind old overlay fs data
		dd if=/dev/zero bs=128k count=1 2>/dev/null
	) | dd of="$OUTFILE" bs=512 seek=$(($1 / 512)) conv=notrunc 2>/dev/null
	printf "Done\n"

	i=$((i+1))
	shift; shift
done
