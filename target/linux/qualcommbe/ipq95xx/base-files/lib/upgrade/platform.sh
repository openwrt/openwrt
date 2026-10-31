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
	nokia,beacon-10)
		[ -n "$(find_mmc_part owrt_kernel mmcblk0)" ] &&
			[ -n "$(find_mmc_part owrt_rootfs mmcblk0)" ] || {
			echo "Beacon 10 OpenWrt partitions not found" >&2
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
	nokia,beacon-10)
		CI_ROOTDEV="mmcblk0"
		CI_KERNPART="owrt_kernel"
		CI_ROOTPART="owrt_rootfs"
		CI_DATAPART=""
		CI_DTBPART=""
		emmc_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}

platform_copy_config() {
	case "$(board_name)" in
	8devices,kiwi-dvk|\
	askey,sbe1v1k|\
	nokia,beacon-10)
		emmc_copy_config
		;;
	esac
}
