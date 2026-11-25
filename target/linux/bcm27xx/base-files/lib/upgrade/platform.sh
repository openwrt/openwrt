. /lib/functions.sh

REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	[ "$#" -gt 1 ] && return 1

	legacy_sdcard_check_image "$1"
}

platform_do_upgrade() {
	legacy_sdcard_do_upgrade "$1"
}

bcm27xx_set_root_part() {
	local root_part

	if [ -f "/boot/partuuid.txt" ]; then
		root_part="PARTUUID=$(cat "/boot/partuuid.txt")-02"
	else
		root_part="/dev/mmcblk0p2"
	fi

	sed -i "s#\broot=[^ ]*#root=${root_part}#g" "/boot/cmdline.txt"
}

platform_copy_config() {
	local partdev

	if export_partdevice partdev 1; then
		mkdir -p /boot
		[ -f /boot/kernel*.img ] || mount -t vfat -o rw,noatime "/dev/$partdev" /boot

		tar -C / -zxvf "$UPGRADE_BACKUP" boot/cmdline.txt boot/config.txt
		bcm27xx_set_root_part

		local backup_tmp="/tmp/backup-update"
		mkdir -p $backup_tmp
		tar -C $backup_tmp -zxvf $UPGRADE_BACKUP
		cp -af /boot/cmdline.txt $backup_tmp/boot/

		local work_dir=$(pwd)
		cd $backup_tmp
		tar -C $backup_tmp -zcvf /boot/$BACKUP_FILE *
		cd $work_dir

		sync
		umount /boot
	fi
}

platform_restore_backup() {
	local TAR_V=$1

	tar -C / -x${TAR_V}zf "$CONF_RESTORE"
	bcm27xx_set_root_part
}
