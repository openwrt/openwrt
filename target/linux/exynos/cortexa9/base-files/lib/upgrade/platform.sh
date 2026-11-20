# SPDX-License-Identifier: GPL-2.0-only

REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	legacy_sdcard_check_image "$1"
}

platform_copy_config() {
	local partdev

	if export_partdevice partdev 1; then
		mount -o rw,noatime "/dev/$partdev" /mnt
		cp -af "$UPGRADE_BACKUP" "/mnt/$BACKUP_FILE"
		umount /mnt
	fi
}

platform_do_upgrade() {
	legacy_sdcard_do_upgrade "$1"
}

platform_legacy_sdcard_post_upgrade() {
	# Update the whole area in front of the first partition (sectors 1..8191):
	# BL1, BL2, U-Boot and TZSW, and reset the U-Boot environment (sector 2560)
	# to the defaults of the new U-Boot
	v "Updating bootloader components..."
	get_image_dd "$IMAGE" of="/dev/$1" bs=512 skip=1 seek=1 count=8191 conv=fsync,notrunc
}
