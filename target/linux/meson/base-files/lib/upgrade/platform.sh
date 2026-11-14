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
	v "Updating bootloader..."
	get_image_dd "$IMAGE" of="/dev/$1" bs=440 count=1 conv=fsync,notrunc
	get_image_dd "$IMAGE" of="/dev/$1" bs=512 skip=1 seek=1 count=8191 conv=fsync,notrunc
}
