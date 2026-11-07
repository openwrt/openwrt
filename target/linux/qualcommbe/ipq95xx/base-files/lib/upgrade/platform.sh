PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv head cmp sha256sum tr mktemp'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

jiorouter_initial_setup() {
	local mtdparts="mtdparts=nand0:0xe100000@0x1700000(ubi)"

	[ "$(fw_printenv -n mtdparts 2>/dev/null)" = "$mtdparts" ] && return 0

	# Point U-Boot at the UBI volumes and set the kernel command line
	fw_setenv -s - <<-EOF
		bootdelay 3
		mtdids nand0=nand0
		mtdparts $mtdparts
		bootcmd ubi part ubi; ubi read 0x44000000 kernel; bootm 0x44000000
		bootargs console=ttyMSM0,115200n8 ubi.mtd=ubi ubi.block=0,rootfs
	EOF
}

platform_check_image() {
	case "$(board_name)" in
	jiorouter,ax6000-jidu6111|\
	jiorouter,ax6000-jidu6411|\
	jiorouter,ax6000-jidu6611|\
	jiorouter,ax6000-jidu6711|\
	jiorouter,ax6000-jidu6811)
		nand_do_platform_check "$(board_name)" "$1"
		return $?
		;;
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
	jiorouter,ax6000-jidu6111|\
	jiorouter,ax6000-jidu6411|\
	jiorouter,ax6000-jidu6611|\
	jiorouter,ax6000-jidu6711|\
	jiorouter,ax6000-jidu6811)
		jiorouter_initial_setup || return 1
		CI_UBIPART="ubi"
		nand_do_upgrade "$1"
		;;
	linksys,ln6001)
		ln6001_do_upgrade "$1" || return 1
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
