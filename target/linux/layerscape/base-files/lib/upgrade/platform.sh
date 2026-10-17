#
# Copyright 2015-2019 Traverse Technologies
# Copyright 2020 NXP
#

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
	local diskdev rootpart bootpart
	local tar_file="$1"
	local tar_opt=""
	local board_dir
	local tmpdir
	local fit_img
	local has_rootfs
	local has_kernel

	if [ "$(identify_magic_long "$(get_magic_long "$tar_file" cat)")" = "gzip" ]; then
		tar_opt="z"
	fi

	board_dir=$(tar t${tar_opt}f "$tar_file" | grep -m 1 '^sysupgrade-.*/$') || {
		echo "Unable to locate sysupgrade payload"
		return 1
	}
	board_dir=${board_dir%/}
	tar t${tar_opt}f "$tar_file" "${board_dir}/root" 1>/dev/null 2>/dev/null && has_rootfs=1
	tar t${tar_opt}f "$tar_file" "${board_dir}/kernel" 1>/dev/null 2>/dev/null && has_kernel=1
	[ "$has_rootfs" = 1 ] || {
		echo "T40 sysupgrade image is missing the rootfs payload"
		return 1
	}
	[ "$has_kernel" = 1 ] || {
		echo "T40 sysupgrade image is missing the FIT payload"
		return 1
	}

	tmpdir="/tmp/t40-sysupgrade"
	rm -rf "$tmpdir"
	mkdir -p "$tmpdir" || return 1
	trap 'rm -rf "$tmpdir"' EXIT HUP INT TERM

	export_bootdevice && export_partdevice diskdev 0 && \
		export_partdevice rootpart 4 && export_partdevice bootpart 3 || {
		echo "Unable to determine T40 upgrade device"
		return 1
	}

	fit_img="$tmpdir/kernel.itb"
	tar x${tar_opt}f "$tar_file" "${board_dir}/kernel" -O > "$fit_img" || return 1

	export T40_ROOT_DEV="/dev/$rootpart"

	echo "Writing squashfs rootfs to $T40_ROOT_DEV..."
	export T40_ROOTFS_BLOCKS=$(($(tar x${tar_opt}f "$tar_file" "${board_dir}/root" -O | dd of="$T40_ROOT_DEV" bs=512 2>&1 | grep "records out" | cut -d' ' -f1)))
	T40_ROOTFS_BLOCKS=$(((T40_ROOTFS_BLOCKS + 127) & ~127))
	sync || return 1
	echo "Updating boot FIT in /dev/$bootpart..."
	mount -t ext2 -o rw,noatime "/dev/$bootpart" /mnt 2>&1 || {
		echo "Unable to mount T40 boot partition"
		return 1
	}
	cp -f "$fit_img" /mnt/kernel_T20_T40_prod.itb.new || {
		umount /mnt
		return 1
	}
	mv -f /mnt/kernel_T20_T40_prod.itb.new /mnt/kernel_T20_T40_prod.itb || {
		umount /mnt
		return 1
	}
	sync || {
		umount /mnt
		return 1
	}
	umount /mnt || return 1
	if [ -z "$UPGRADE_BACKUP" ] && [ "$T40_ROOTFS_BLOCKS" ]; then
		dd if=/dev/zero of="$T40_ROOT_DEV" bs=512 seek=$T40_ROOTFS_BLOCKS count=8 || return 1
	fi
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
		echo "Saving config backup..."
		[ "$T40_ROOT_DEV" -a "$T40_ROOTFS_BLOCKS" ] || {
			echo "Unable to determine T40 config destination"
			return 1
		}
		dd if="$UPGRADE_BACKUP" of="$T40_ROOT_DEV" bs=512 seek=$T40_ROOTFS_BLOCKS || return 1
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
