platform_check_image() {
	# i know no way to verify the image
	return 0;
}

platform_do_upgrade() {
	sync
	get_image "$1" | dd of=/dev/mmcblk0 bs=2M conv=fsync
	sleep 1
}

platform_copy_config() {
	mount -t vfat -o rw,noatime /dev/mmcblk0p1 /mnt
	cp -af "$CONF_TAR" /mnt/
	sync
	umount /mnt
}
