#!/bin/sh

. /lib/functions.sh
. /lib/upgrade/common.sh

watchguard_t30_do_upgrade() {
	local tar_file="$1"
	local rootfs bootfs disk overlay_part

	# Partition discovery: the SD card is MBR, so there are no partition
	# names for find_mmc_part() to match — derive both from the kernel root=.
	rootfs="$(sed -n 's/.*[[:space:]]root=\([^ \t]*\).*/\1/p' /proc/cmdline)"
	if [ -z "$rootfs" ]; then
		rootfs="/dev/mmcblk0p2"
	fi
	disk="${rootfs%p*}"
	bootfs="${disk}p3"

	if [ ! -b "$rootfs" ] || [ ! -b "$bootfs" ]; then
		echo "Error: Could not find rootfs ($rootfs) or bootfs ($bootfs) partitions."
		return 1
	fi

	# 1. Update kernel in boot partition (p3)
	get_image "$tar_file" | tar -O -xf - sysupgrade-watchguard_firebox-t30/kernel > /tmp/kernel.img 2>/dev/null
	if [ ! -s /tmp/kernel.img ]; then
		echo "Error: Failed to extract kernel."
		return 1
	fi
	dd if=/tmp/kernel.img of="$bootfs" bs=4M conv=fsync status=none
	rm -f /tmp/kernel.img

	# 2. Extract rootfs to rootfs partition (p2)
	get_image "$tar_file" | tar -O -xf - sysupgrade-watchguard_firebox-t30/root > /tmp/root.img 2>/dev/null
	if [ ! -s /tmp/root.img ]; then
		echo "Error: Failed to extract root."
		return 1
	fi
	dd if=/tmp/root.img of="$rootfs" bs=4M conv=fsync status=none
	rm -f /tmp/root.img

	# 3. Handle sysupgrade -n (wipe and format overlay partition p4 if no backup)
	if [ ! -f "$UPGRADE_BACKUP" ]; then
		overlay_part="${disk}p4"
		if [ -b "$overlay_part" ]; then
			echo "Erasing overlay partition $overlay_part..."
			dd if=/dev/zero of="$overlay_part" bs=1M count=10 status=none
			echo "Formatting overlay partition $overlay_part..."
			mkfs.ext4 -q -F -L rootfs_data "$overlay_part"
		fi
	fi

	sync
	return 0
}
