# SPDX-License-Identifier: GPL-2.0-or-later

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

platform_copy_config_sdboot() {
	local diskdev partdev parttype=ext4

	export_bootdevice && export_partdevice diskdev 0 || {
		echo "Unable to determine upgrade device"
		return 1
	}

	if export_partdevice partdev 1; then
		mount -t $parttype -o rw,noatime "/dev/$partdev" /mnt 2>&1
		echo "Saving config backup..."
		cp -af "$UPGRADE_BACKUP" "/mnt/$BACKUP_FILE"
		umount /mnt
	fi
}

platform_do_upgrade_sdboot() {
	local diskdev partdev parttype=ext4
	local tar_file="$1"
	local board_dir=$(tar tf $tar_file | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}

	export_bootdevice && export_partdevice diskdev 0 || {
		echo "Unable to determine upgrade device"
		return 1
	}

	if export_partdevice partdev 1; then
		mount -t $parttype -o rw,noatime "/dev/$partdev" /mnt 2>&1
		echo "Writing kernel..."
		tar xf $tar_file ${board_dir}/kernel -O > /mnt/fitImage
		umount /mnt
	fi

	echo "Erasing rootfs..."
	dd if=/dev/zero of=/dev/mmcblk0p2 bs=1M > /dev/null 2>&1
	echo "Writing rootfs..."
	tar xf $tar_file ${board_dir}/root -O  | dd of=/dev/mmcblk0p2 bs=512k > /dev/null 2>&1

}

platform_check_image() {
	case "$(board_name)" in
	fsl,t4240rdb-sdboot)
		return 0
		;;
	watchguard,firebox-m300)
		legacy_sdcard_check_image "$1"
		;;
	*)
		return 0
		;;
	esac
}

platform_copy_config() {
	case "$(board_name)" in
	fsl,t4240rdb-sdboot)
		platform_copy_config_sdboot
		;;
	watchguard,firebox-m300)
		legacy_sdcard_copy_config "$1"
		;;
	*)
		return 0
	esac
}

platform_do_upgrade() {
	case "$(board_name)" in
	fsl,t4240rdb-sdboot)
		platform_do_upgrade_sdboot "$1"
		return 0
		;;
	watchguard,firebox-m300)
		legacy_sdcard_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}

