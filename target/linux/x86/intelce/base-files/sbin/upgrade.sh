#!/bin/sh

TMP_PATH=/tmp

KERNEL=$TMP_PATH/openwrt-x86-intelce-intelce-kernel.bin
ROOTFS=$TMP_PATH/openwrt-x86-intelce-intelce-squashfs-rootfs.img

KERNEL_PART=/dev/mmcblk0p1
ROOTFS_PART=/dev/mmcblk0p3

OPTS="bs=16M"

echo "Source kernel: $KERNEL"
echo "Source rootfs: $ROOTFS"

echo "Kernel part is $KERNEL_PART"
echo "rootfs part is $ROOTFS_PART"

# grab reboot and sync
REBOOT=$(which reboot)
SYNC=$(which sync)

echo "Grabbing reboot from $REBOOT"
cp $REBOOT $TMP_PATH/
echo "Grabbing sync from $SYNC"
cp $SYNC $TMP_PATH/

echo "Updating kernel, opts = $OPTS"
dd if=$KERNEL of=$KERNEL_PART $OPTS
echo "Updating rootfs, opts = $OPTS"
dd if=$ROOTFS of=$ROOTFS_PART $OPTS
echo "Syncing"
$TMP_PATH/sync
echo "Rebooting now!"
$TMP_PATH/reboot -f
