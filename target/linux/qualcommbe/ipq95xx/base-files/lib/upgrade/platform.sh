PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv head'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

be7000_do_upgrade() {
	local image="$1"
	local mtdnum target_slot target_part

	# Upgrade the slot we are running from and never touch the other one.
	#
	# The other slot holds the stock firmware, and it is the only recovery
	# path there is: after a few failed boots the bootloader falls back to it.
	# The previous version always wrote rootfs_1, which silently destroyed
	# that fallback on units where OpenWrt had been installed into slot 0.
	#
	# The running slot comes from the kernel command line, which the
	# bootloader fills in, so it reflects what actually booted rather than
	# what the flags claim.
	#
	# Matched word by word on purpose: the command line carries a second
	# ubi.mtd= for the spare overlay partition, and a greedy match would
	# return that one instead of the slot.
	target_part=""
	for arg in $(cat /proc/cmdline); do
		case "$arg" in
		ubi.mtd=rootfs)   target_part="rootfs";   target_slot=0 ;;
		ubi.mtd=rootfs_1) target_part="rootfs_1"; target_slot=1 ;;
		esac
	done
	if [ -z "$target_part" ]; then
		echo "Cannot tell which slot is running from /proc/cmdline, refusing to flash"
		return 1
	fi

	# Escape hatch for deliberately staging an image into the other slot, e.g.
	# when testing a kernel that may not boot. Not used by a normal upgrade.
	case "$BE7000_TARGET_SLOT" in
	0) target_slot=0; target_part="rootfs" ;;
	1) target_slot=1; target_part="rootfs_1" ;;
	esac

	mtdnum="$(find_mtd_index "$target_part")"
	if [ -z "$mtdnum" ]; then
		echo "Unable to find target partition $target_part"
		return 1
	fi

	if ! command -v fw_setenv >/dev/null; then
		echo "fw_setenv is required to switch BE7000 boot slot"
		return 1
	fi

	echo "Writing OpenWrt image to Xiaomi OpenWrt UBI slot $target_part (slot $target_slot)"
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
