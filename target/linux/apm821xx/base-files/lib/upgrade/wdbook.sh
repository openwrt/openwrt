. /lib/functions.sh

mbl_copy_config() {
	local partdev

	if export_partdevice partdev 1; then
		mount -t ext4 -o rw,noatime "/dev/$partdev" /mnt
		cp -af "$UPGRADE_BACKUP" "/mnt/$BACKUP_FILE"
		umount /mnt
	fi
}
