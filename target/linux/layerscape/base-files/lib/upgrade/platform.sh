#
# Copyright 2015-2019 Traverse Technologies
# Copyright 2020 NXP
#

RAMFS_COPY_BIN=""
RAMFS_COPY_DATA=""

REQUIRE_IMAGE_METADATA=1

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

platform_do_upgrade_traverse_slotubi() {
	part="$(awk -F 'ubi.mtd=' '{printf $2}' /proc/cmdline | sed -e 's/ .*$//')"
	echo "Active boot slot: ${part}"
	new_active_sys="b"

	if [ ! -z "${part}" ]; then
		if [ "${part}" = "ubia" ]; then
			CI_UBIPART="ubib"
		else
			CI_UBIPART="ubia"
			new_active_sys="a"
		fi
	fi
	echo "Updating UBI part ${CI_UBIPART}"
	fw_setenv "openwrt_active_sys" "${new_active_sys}"
	nand_do_upgrade "$1"
	return $?
}

platform_do_upgrade_t40() {
	local diskdev partdev
	local tar_file="$1"
	local tar_opt=""
	local board_dir
	local rootfs_img="$tar_file"
	local tmpdir
	local rootfs_magic

	if gzip -t "$tar_file" >/dev/null 2>&1; then
		tar_opt="z"
	fi

	board_dir=$(tar t${tar_opt}f "$tar_file" | grep -m 1 '^sysupgrade-.*/$') || {
		echo "Unable to locate sysupgrade payload"
		return 1
	}
	board_dir=${board_dir%/}
	tmpdir="/tmp/t40-sysupgrade"
	rm -rf "$tmpdir"
	mkdir -p "$tmpdir" || return 1
	trap 'rm -rf "$tmpdir"' EXIT HUP INT TERM

	# On this board U-Boot boots directly from the SATA SYSA partition, so the
	# actual OpenWrt upgrade target is always partition 3 of the current boot disk.
	# We intentionally do not write any other SSD partitions here.
	export_bootdevice && export_partdevice diskdev 0 && export_partdevice partdev 3 || {
		echo "Unable to determine T40 upgrade device"
		return 1
	}

	rootfs_img="$tmpdir/root.img"
	tar x${tar_opt}f "$tar_file" "${board_dir}/root" -O > "$rootfs_img" || return 1
	rootfs_magic="$(dd if="$rootfs_img" bs=2 count=1 2>/dev/null | hexdump -v -e '1/1 "%02x"')"

	echo "Writing rootfs to /dev/$partdev..."
	# Extract the archived SYSA image, verify it and stream it directly to sda3.
	# The image already contains /kernel_T20_T40_prod.itb at the root of the new
	# filesystem, so there is no separate kernel write step during sysupgrade.
	case "$rootfs_magic" in
	1f8b)
		gzip -t "$rootfs_img" || {
			echo "Invalid compressed SYSA payload"
			return 1
		}
		gzip -dc "$rootfs_img" | dd of="/dev/$partdev" bs=4M conv=fsync || return 1
		;;
	*)
		dd if="$rootfs_img" of="/dev/$partdev" bs=4M conv=fsync || return 1
		;;
	esac
	sync || return 1
	rm -rf "$tmpdir"
	trap - EXIT HUP INT TERM
}

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
platform_copy_config() {
	local board=$(board_name)

	case "$board" in
	watchguard,firebox-t40)
		local diskdev partdev

		# The generic sysupgrade flow saves the backup after platform_do_upgrade().
		# For the T40 we therefore remount the freshly written SYSA rootfs and drop
		# /sysupgrade.tgz into its root directory. Preinit restores it automatically
		# on the next boot.
		export_bootdevice && export_partdevice diskdev 0 && export_partdevice partdev 3 || {
			echo "Unable to determine T40 config destination"
			return 1
		}

		mount -t ext2 -o rw,noatime "/dev/$partdev" /mnt 2>&1 || return 1
		echo "Saving config backup..."
		cp -af "$UPGRADE_BACKUP" "/mnt/$BACKUP_FILE" || {
			umount /mnt
			return 1
		}
		sync || {
			umount /mnt
			return 1
		}
		umount /mnt || return 1
		;;
	fsl,ls1012a-frwy-sdboot | \
	fsl,ls1021a-iot-sdboot | \
	fsl,ls1021a-twr-sdboot | \
	fsl,ls1028a-rdb-sdboot | \
	fsl,ls1043a-rdb-sdboot | \
	fsl,ls1046a-frwy-sdboot | \
	fsl,ls1046a-rdb-sdboot | \
	fsl,ls1088a-rdb-sdboot | \
	fsl,lx2160a-rdb-sdboot)
		platform_copy_config_sdboot
		;;
	esac
}
platform_check_image() {
	local board=$(board_name)

	case "$board" in
	traverse,ten64)
		nand_do_platform_check "ten64-mtd" $1
		return $?
		;;
	fsl,ls1012a-frdm | \
	fsl,ls1012a-frwy-sdboot | \
	fsl,ls1012a-rdb | \
	fsl,ls1021a-iot-sdboot | \
	fsl,ls1021a-twr | \
	fsl,ls1021a-twr-sdboot | \
	fsl,ls1028a-rdb | \
	fsl,ls1028a-rdb-sdboot | \
	watchguard,firebox-t40 | \
	fsl,ls1043a-rdb | \
	fsl,ls1043a-rdb-sdboot | \
	fsl,ls1046a-frwy | \
	fsl,ls1046a-frwy-sdboot | \
	fsl,ls1046a-rdb | \
	fsl,ls1046a-rdb-sdboot | \
	fsl,ls1088a-rdb | \
	fsl,ls1088a-rdb-sdboot | \
	fsl,ls2088a-rdb | \
	fsl,lx2160a-rdb | \
	fsl,lx2160a-rdb-sdboot)
		return 0
		;;
	*)
		echo "Sysupgrade is not currently supported on $board"
		;;
	esac

	return 1
}
platform_do_upgrade() {
	local board=$(board_name)

	# Force the creation of fw_printenv.lock
	mkdir -p /var/lock
	touch /var/lock/fw_printenv.lock

	case "$board" in
	traverse,ten64)
		platform_do_upgrade_traverse_slotubi "${1}"
		;;
	watchguard,firebox-t40)
		platform_do_upgrade_t40 "${1}"
		;;
	fsl,ls1012a-frdm | \
	fsl,ls1012a-rdb | \
	fsl,ls1021a-twr | \
	fsl,ls1028a-rdb | \
	fsl,ls1043a-rdb | \
	fsl,ls1046a-frwy | \
	fsl,ls1046a-rdb | \
	fsl,ls1088a-rdb | \
	fsl,ls2088a-rdb | \
	fsl,lx2160a-rdb)
		PART_NAME=firmware
		default_do_upgrade "$1"
		;;
	fsl,ls1012a-frwy-sdboot | \
	fsl,ls1021a-iot-sdboot | \
	fsl,ls1021a-twr-sdboot | \
	fsl,ls1028a-rdb-sdboot | \
	fsl,ls1043a-rdb-sdboot | \
	fsl,ls1046a-frwy-sdboot | \
	fsl,ls1046a-rdb-sdboot | \
	fsl,ls1088a-rdb-sdboot | \
	fsl,lx2160a-rdb-sdboot)
		platform_do_upgrade_sdboot "$1"
		return 0
		;;
	*)
		echo "Sysupgrade is not currently supported on $board"
		;;
	esac
}
