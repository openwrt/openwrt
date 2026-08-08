PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv head'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

be7000_do_upgrade() {
	local image="$1"
	local mtdnum
	local target_slot=1
	local target_part="rootfs_1"

	mtdnum="$(find_mtd_index "$target_part")"
	if [ -z "$mtdnum" ]; then
		echo "Unable to find target partition $target_part"
		return 1
	fi

	if ! command -v fw_setenv >/dev/null; then
		echo "fw_setenv is required to switch BE7000 boot slot"
		return 1
	fi

	echo "Writing OpenWrt image to Xiaomi OpenWrt UBI slot $target_part"
	CI_UBIPART="$target_part"
	CI_ROOTPART="ubi_rootfs"
	nand_detach_ubi "$target_part" || return 1
	ubiformat "/dev/mtd$mtdnum" -y || return 1
	nand_do_flash_file "$image" "fwtool -q -i /tmp/sysupgrade.meta -T $image" || return 1

	echo "Switching Xiaomi boot slot to $target_slot"
	fw_setenv flag_boot_rootfs "$target_slot" || return 1
	fw_setenv flag_last_success "$target_slot" || return 1
	fw_setenv flag_boot_success 1 || return 1
	fw_setenv flag_try_sys1_failed 0 || return 1
	fw_setenv flag_try_sys2_failed 0 || return 1
	fw_setenv flag_ota_reboot 0 || return 1

	nand_do_restore_config || return 1
	sync

	return 0
}

platform_check_image() {
	return 0;
}

platform_do_upgrade() {
	case "$(board_name)" in
	8devices,kiwi-dvk)
		CI_KERNPART="0:HLOS"
		CI_ROOTPART="rootfs"
		emmc_do_upgrade "$1"
		;;
	xiaomi,be7000)
		sync
		be7000_do_upgrade "$1" && {
			echo "sysupgrade successful"
			umount -a
			reboot -f
		}
		echo "sysupgrade failed"
		return 1
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}

platform_copy_config() {
	case "$(board_name)" in
	8devices,kiwi-dvk)
		emmc_copy_config
		;;
	esac
}
