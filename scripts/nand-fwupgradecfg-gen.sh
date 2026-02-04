#!/bin/sh
#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

usage() {
	echo "Usage: $0 <TARGET> <out file path> <kernel path> <rootfs path>"
	exit 1
}

[ "$#" -lt 4 ] && usage

CE_TYPE=$1
CFG_OUT=$2
KERNEL_PATH=$3
ROOTFS_PATH=$4

case $CE_TYPE in
PAX1800-Lite)
	EXTRA_SETENV=""
	;;
*)
	echo "Error - unsupported ce type: $CE_TYPE"
	exit 1
	;;
esac

# kernel calculation
KERNEL_MD5=$(mkhash md5 $KERNEL_PATH)
KERNEL_SHA256=$(mkhash sha256 $KERNEL_PATH)
KERNEL_SIZE=$(stat -c%s "$KERNEL_PATH")
KERNEL_CHECK_SIZE=$(printf '0x%x' $KERNEL_SIZE)

# rootfs calculation
ROOTFS_MD5=$(mkhash md5 $ROOTFS_PATH)
ROOTFS_SHA256=$(mkhash sha256 $ROOTFS_PATH)
ROOTFS_SIZE=$(stat -c%s "$ROOTFS_PATH")
ROOTFS_CHECK_SIZE=$(printf '0x%x' $ROOTFS_SIZE)

cat << EOF > $CFG_OUT
[kernel]
filename=kernel
md5sum=$KERNEL_MD5
filemd5sum=$KERNEL_MD5
filesha256sum=$KERNEL_SHA256
checksize=$KERNEL_CHECK_SIZE
ubi=1
cmd_success=setenv bootseq 1,2; $EXTRA_SETENV; saveenv

[rootfs]
filename=rootfs
md5sum=$ROOTFS_MD5
filemd5sum=$ROOTFS_MD5
filesha256sum=$ROOTFS_SHA256
checksize=$ROOTFS_CHECK_SIZE
ubi=1
cmd_success=setenv bootseq 1,2; $EXTRA_SETENV; saveenv
EOF
