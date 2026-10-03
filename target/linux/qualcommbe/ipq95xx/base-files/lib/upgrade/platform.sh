PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv head cmp sha256sum tr mktemp'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

platform_check_image() {
	case "$(board_name)" in
	linksys,ln6001)
		ln6001_preflight "$1" || {
			echo "LN6001 A/B preflight failed" >&2
			return 1
		}
		return 0
		;;
	tplink,archer-be800|tplink,archer-be800-sfp)
		# sysupgrade must be able to switch the u-boot boot slot; an
		# unreadable environment would leave the new image stranded on
		# the inactive partition with the router still booting the old
		# one. Fail the upgrade before rebooting if it cannot work.
		tplink_regenerate_fw_env_config || {
			echo "cannot locate '0:appsblenv' MTD partition"
			return 1
		}
		fw_printenv >/dev/null 2>&1 || {
			echo "u-boot environment unreadable via /etc/fw_env.config"
			return 1
		}
		;;
	esac

	return 0
}

# Write /etc/fw_env.config for the u-boot environment partition, resolved by
# MTD partition NAME at upgrade time. The numeric MTD index of 0:appsblenv
# shifts between kernel versions; a stale index would aim fw_setenv writes
# at an unrelated partition.
tplink_regenerate_fw_env_config() {
	local envmtd

	envmtd=$(awk -F: '/"0:appsblenv"/{sub("mtd","", $1); print $1; exit}' /proc/mtd)
	[ -n "$envmtd" ] || return 1

	echo "/dev/mtd${envmtd} 0x0000 0x00040000 0x00020000" > /etc/fw_env.config
}

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

tplink_get_boot_part() {
	local cur_boot_part
	local args

	# Try to find rootfs from kernel arguments
	read -r args < /proc/cmdline
	for arg in $args; do
		local ubi_mtd_arg=${arg#ubi.mtd=}
		case "$ubi_mtd_arg" in
		rootfs|rootfs_1)
			echo "$ubi_mtd_arg"
			return
		;;
		esac
	done

	# Fallback to u-boot env (e.g. when running initramfs)
	cur_boot_part="$(/usr/sbin/fw_printenv -n tp_boot_idx)"
	case $cur_boot_part in
	1)
		echo rootfs_1
		;;
	0|*)
		echo rootfs
		;;
	esac
}

tplink_do_upgrade() {
	local new_boot_part

	# Regenerate the environment map inside the upgrade ramfs too: the
	# copied /etc/fw_env.config may predate the currently running kernel.
	tplink_regenerate_fw_env_config || {
		echo "cannot locate '0:appsblenv' MTD partition; aborting"
		exit 1
	}

	case $(tplink_get_boot_part) in
	rootfs)
		CI_UBIPART="rootfs_1"
		new_boot_part=1
	;;
	rootfs_1)
		CI_UBIPART="rootfs"
		new_boot_part=0
	;;
	esac

	remove_oem_ubi_volume ubi_rootfs
	sync
	nand_do_flash_file "$1" || nand_do_upgrade_failed

	# Switch the boot slot only after a good write, so an interrupted
	# upgrade leaves u-boot on the old, intact system.
	fw_setenv "tp_boot_idx" "$new_boot_part" || {
		echo "fw_setenv failed to set tp_boot_idx"
		nand_do_upgrade_failed
	}

	nand_do_upgrade_success
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
	tplink,archer-be800|tplink,archer-be800-sfp)
		tplink_do_upgrade "$1"
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
