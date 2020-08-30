RAMFS_COPY_BIN='fw_printenv fw_setenv'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	local board="$(board_name)"

	case "$board" in
	netgear,readynas-duo-v2)
		# let's store how rootfs is mounted
		cp /proc/mounts /tmp/mounts
		return 0
		;;
	*)
		return 0
		;;
	esac
}

platform_do_upgrade() {
	local board="$(board_name)"

	case "$board" in
	linksys,e4200-v2|\
	linksys,ea3500|\
	linksys,ea4500)
		platform_do_upgrade_linksys "$1"
		;;
	netgear,readynas-duo-v2)
		local root_mtd=$(cat /tmp/mounts | grep "/dev/root")
		if [ -z "$root_mtd" ]; then
			mkdir -p /var/lock
			touch /var/lock/fw_printenv.lock
			fw_setenv bootargs 'console=ttyS0,115200n8 earlyprintk'
			fw_setenv bootcmd 'nand read.e 0x1200000 0x200000 0x600000;bootm 0x1200000'
		fi
		nand_do_upgrade "$1"
		;;
	*)
		nand_do_upgrade "$1"
		;;
	esac
}
