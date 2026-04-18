REQUIRE_IMAGE_METADATA=1

export_partdevice_by_name() {
	local var="$1" partname="$2"
	local uevent line MAJOR MINOR DEVNAME DEVTYPE DISKSEQ PARTN PARTNAME PARTUUID

	for uevent in /sys/class/block/*/uevent; do
		while read line; do
			export -n "$line"
		done < "$uevent"
		if [ "$BOOTDEV_DISKSEQ" = "$DISKSEQ" -a -b "/dev/$DEVNAME" ]; then
			if [ "$PARTNAME" = "$partname" -a "$DEVTYPE" = "partition" ]; then
				export "$var=$DEVNAME"
				return 0
			fi
		fi
	done

	return 1
}

platform_check_image() {
	v "Running on board: $(board_name)"

	legacy_sdcard_check_image "$1"
}

platform_copy_config() {
	local partdev

	if export_partdevice_by_name partdev "bootfs"; then
		v "Storing the configuration on /dev/$partdev"
		mount -o rw,noatime "/dev/$partdev" /mnt
		cp -af "$UPGRADE_BACKUP" "/mnt/$BACKUP_FILE"
		umount /mnt
	else
		v "ERROR: Unable to find the partition to store the configuration on"
	fi
}

platform_do_upgrade() {
	legacy_sdcard_do_upgrade "$1"
}
