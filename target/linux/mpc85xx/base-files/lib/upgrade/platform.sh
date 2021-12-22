#
# Copyright (C) 2011 OpenWrt.org
#

RAMFS_COPY_BIN="/usr/sbin/fw_printenv /usr/sbin/fw_setenv /usr/sbin/ubinfo /bin/echo"
RAMFS_COPY_DATA="/etc/fw_env.config /var/lock/fw_printenv.lock"

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	return 0
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	ocedo,panda|\
	sophos,red-15w-rev1)
		nand_do_upgrade "$1"
		;;
	aerohive,hiveap-330)
		fw_setenv owrt_boot 'bootm 0xEC040000 - 0xEC000000;'
		default_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
