#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Copyright (c) 2021 Mikhail Zhilkin <csharper2005@gmail.com>
#
###
### sercomm-kernel.sh  - calculates and appends a special kernel header.
###                      Intended for some Sercomm devices (e.g., Beeline
###                      SmartBox GIGA).
###
#
# Thanks to @kar200 for header format description. More details here:
# https://forum.openwrt.org/t/add-support-for-sercomm-s3-on-stock-uboot
#
if [ $# -ne 3 ]; then
	echo "SYNTAX: $0 <kernel> <kernel_offset> <rootfs_offset>"
	exit 1
fi

KERNEL_IMG=$1
KERNEL_OFFSET=$2
ROOTFS_OFFSET=$3
FILE_TMP=$1.shdr

add_zeros () {
	awk '{ printf "%8s\n", $0 }' | sed 's/ /0/g'
}

swap_bytes () {
	add_zeros | awk '{for (i=7;i>=1;i=i-2) printf "%s%s", \
		substr($1,i,2), (i>1?"":"\n")}'
}

get_rev_fsize () {
	printf "%x\n" `stat -c "%s" $1` | swap_bytes
}

get_rev_crc () {
	dd if=$1 2>/dev/null | gzip -c | tail -c 8 | od -An -tx4 -N4 \
		--endian=big | tr -d ' \n' | add_zeros
}

# Write 4 bytes in the header by offset
write_hdr () {
	echo -ne "$(echo $1 | sed 's/../\\x&/g')" | dd of=$FILE_TMP bs=1 \
	seek=$(($2)) count=4 conv=notrunc status=none 2>/dev/null
}

# Write 256 bytes of 0xFF to the header
dd if=/dev/zero ibs=1 count=256 status=none | tr "\000" "\377" > \
	$FILE_TMP 2>/dev/null

# Write constant values
write_hdr 00000000 $((0x1c)); write_hdr 00000000 $((0x34))
write_hdr 02ffffff $((0xc));
# "RootFS" size. We check 4 first bytes only
write_hdr 04000000 $((0x2c))
# "RootFS" end offset
write_hdr $(printf "%x\n" $((16#$ROOTFS_OFFSET + 16#$((0x4)))) | \
	swap_bytes) $((0x4))
write_hdr $(echo $KERNEL_OFFSET | swap_bytes) $((0x10))
write_hdr $(echo $ROOTFS_OFFSET | swap_bytes) $((0x28))
# Kernel size & checksum
write_hdr $(get_rev_fsize $KERNEL_IMG) $((0x14))
write_hdr $(get_rev_crc $KERNEL_IMG) $((0x18))
# "RootFS" (first 4 bytes) checksum. It's constant for UBI.
write_hdr 1cfc552d $((0x30))
# Header checksum
write_hdr $(get_rev_crc $FILE_TMP) $((0x8))
# Sercomm signature
write_hdr 53657200 $((0x0))

dd if=$KERNEL_IMG >> $FILE_TMP
mv $FILE_TMP $KERNEL_IMG
