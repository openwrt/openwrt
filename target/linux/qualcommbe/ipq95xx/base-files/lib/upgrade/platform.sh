PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv head cmp sha256sum tr mktemp'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

remove_oem_ubi_volume() {
	local oem_volume_name="$1"
	local oem_ubivol
	local mtdnum
	local ubidev

	mtdnum=$(find_mtd_index "$CI_UBIPART")
	if [ ! "$mtdnum" ]; then
		return
	fi

	ubidev=$(nand_find_ubi "$CI_UBIPART")
	if [ ! "$ubidev" ]; then
		ubiattach --mtdn="$mtdnum"
		ubidev=$(nand_find_ubi "$CI_UBIPART")
	fi

	if [ "$ubidev" ]; then
		oem_ubivol=$(nand_find_volume "$ubidev" "$oem_volume_name")
		[ "$oem_ubivol" ] && ubirmvol "/dev/$ubidev" --name="$oem_volume_name"
	fi
}

platform_check_image() {
	case "$(board_name)" in
	linksys,ln6001)
		ln6001_preflight "$1" || {
			echo "LN6001 A/B preflight failed" >&2
			return 1
		}
		return 0
		;;
	*)
		return 0
		;;
	esac
}

platform_do_upgrade() {
	case "$(board_name)" in
	8devices,kiwi-dvk)
		CI_KERNPART="0:HLOS"
		CI_ROOTPART="rootfs"
		emmc_do_upgrade "$1"
		;;
	askey,sbe1v1k)
		CI_KERNPART="0:HLOS"
		CI_ROOTPART="rootfs"
		CI_DATAPART="rootfs_data"
		emmc_do_upgrade "$1"
		;;
	linksys,ln6001)
		ln6001_do_upgrade "$1" || return 1
		;;
	tplink,archer-be550-v1)
		# stock shows its "upgrade" animation while flashing, unless the
		# LEDs are switched off
		for f in /sys/bus/i2c/drivers/tplink-ledbar/*/effect; do
			[ -w "$f" ] && [ "$(cat "$f")" != off ] && echo upgrade > "$f"
		done
		# Dual boot: install into the inactive rootfs/rootfs_1 slot
		# and point tp_boot_idx at it only once it is fully written.
		# If the selected slot fails to load, TP-Link's U-Boot tries
		# the other one but keeps the failed slot's ubi.mtd= in the
		# bootargs, so an interrupted write must never be selected.
		local idx=1
		CI_UBIPART="rootfs_1"
		if grep -q 'ubi.mtd=rootfs_1' /proc/cmdline; then
			idx=0
			CI_UBIPART="rootfs"
		fi
		# a slot last written by the stock firmware holds its
		# "ubi_rootfs" volume, which would leave no room for ours
		remove_oem_ubi_volume ubi_rootfs
		sync
		nand_do_flash_file "$1" || nand_do_upgrade_failed
		fw_setenv tp_boot_idx $idx || {
			echo "failed to set tp_boot_idx $idx"
			nand_do_upgrade_failed
		}
		nand_do_upgrade_success
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}

platform_copy_config() {
	case "$(board_name)" in
	8devices,kiwi-dvk|\
	askey,sbe1v1k)
		emmc_copy_config
		;;
	esac
}
