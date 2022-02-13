RAMFS_COPY_BIN='fdisk fw_printenv fw_setenv mkfs.ext3 mkfs.ext4 strings'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	local board="$(board_name)"

	case "$board" in
	iodata,hdl-a|\
	iodata,hdl2-a)
		iodata_check_image "$1" || return 1
		return 0
		;;
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
	ctera,c200-v1)
		part=$(find_mtd_part "active_bank")

		if [ -n "$part" ]; then
			CI_KERNPART="$(strings $part | grep bank)"
			nand_do_upgrade "$1"
		else
			echo "active_bank partition missed!"
			return 1
		fi
		;;
	iodata,hdl-a|\
	iodata,hdl2-a)
		iodata_do_upgrade "l2a" "$1"
		;;
	iptime,nas1)
		default_do_upgrade "$1"
		;;
	linksys,e4200-v2|\
	linksys,ea3500|\
	linksys,ea4500)
		platform_do_upgrade_linksys "$1"
		;;
	*)
		nand_do_upgrade "$1"
		;;
	esac
}
