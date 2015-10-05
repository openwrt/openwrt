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
	mkdir -p /boot
	[ -f /boot/kernel.img ] || mount -t vfat -o rw,noatime /dev/mmcblk0p1 /boot
	cp -af "$CONF_TAR" /boot/
	sync
	umount /boot
}
