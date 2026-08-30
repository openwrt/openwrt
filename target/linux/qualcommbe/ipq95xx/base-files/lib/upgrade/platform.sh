PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv head'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

jiorouter_initial_setup() {
	[ "$(rootfs_type)" = "tmpfs" ] || return 0

	local mtdnum="$(find_mtd_index 0:APPSBLENV)"
	if [ ! "$mtdnum" ]; then
		echo "unable to find mtd partition 0:APPSBLENV"
		return 1
	fi

	echo "/dev/mtd$mtdnum 0x0 0x40000 0x20000" > /etc/fw_env.config

	# Set up U-Boot environment to boot OpenWrt
	fw_setenv mtdids 'nand0=nand0' || return 1
	fw_setenv mtdparts 'mtdparts=nand0:0xe100000@0x1700000(ubi)' || return 1
	fw_setenv bootcmd 'ubi part ubi; ubi read 0x44000000 kernel; bootm 0x44000000' || return 1
	fw_setenv bootargs 'console=ttyMSM0,115200n8 ubi.mtd=ubi root=/dev/ubiblock0_1 rootfstype=squashfs' || return 1
}

platform_do_upgrade() {
	case "$(board_name)" in
	jiorouter,ax6000-jidu6j11-6111|\
	jiorouter,ax6000-jidu6j11-6411|\
	jiorouter,ax6000-jidu6j11-6611|\
	jiorouter,ax6000-jidu6j11-6811|\
	jiorouter,ax6000-jidu6j11-6911)
		jiorouter_initial_setup || return 1
		CI_UBIPART="ubi"
		nand_do_upgrade "$1"
		;;
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
	*)
		default_do_upgrade "$1"
		;;
	esac
}

platform_check_image() {
	local board=$(board_name)

	case "$board" in
	jiorouter,ax6000-jidu6j11-6111|\
	jiorouter,ax6000-jidu6j11-6411|\
	jiorouter,ax6000-jidu6j11-6611|\
	jiorouter,ax6000-jidu6j11-6811|\
	jiorouter,ax6000-jidu6j11-6911)
		nand_do_platform_check "$board" "$1"
		return $?
		;;
	*)
		return 0
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
