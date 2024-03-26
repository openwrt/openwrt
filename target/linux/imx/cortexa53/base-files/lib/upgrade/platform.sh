
enable_image_metadata_check() {
	case "$(board_name)" in
	gw,imx8m*)
		REQUIRE_IMAGE_METADATA=1
		;;
	esac
}
enable_image_metadata_check

platform_check_image() {
	local board=$(board_name)

	case "$board" in
	gw,imx8m*)
		return 0
		;;
	esac

	echo "Sysupgrade is not yet supported on $board."
	return 1
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	gw,imx8m*)
		export_bootdevice && export_partdevice diskdev 0 || {
			echo "Unable to find root device."
			return 1
		}
		v "Updating /dev/$diskdev..."
		get_image "$@" | dd of="/dev/$diskdev" bs=4096 conv=fsync
		;;
	esac
}

platform_copy_config() {
	local board=$(board_name)
	local partdev

	case "$board" in
	gw,imx8m*)
		export_partdevice partdev 1 && {
			v "Storing $UPGRADE_BACKUP on /dev/$partdev..."
			mount -o rw,noatime "/dev/$partdev" /mnt
			cp -af "$UPGRADE_BACKUP" "/mnt/$BACKUP_FILE"
			umount /mnt
		}
		;;
	esac
}
