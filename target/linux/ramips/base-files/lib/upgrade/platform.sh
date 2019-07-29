#
# Copyright (C) 2010 OpenWrt.org
#

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

RAMFS_COPY_BIN='fw_printenv fw_setenv'
RAMFS_COPY_DATA='/etc/fw_env.config /var/lock/fw_printenv.lock'

platform_check_image() {
	return 0
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	mikrotik,rb750gr3|\
	mikrotik,rbm11g|\
	mikrotik,rbm33g)
		[ -z "$(rootfs_type)" ] && mtd erase firmware
		;;
	esac

	case "$board" in
	hiwifi,hc5962|\
	netgear,r6220|\
	netgear,r6350|\
	xiaomi,mir3g|\
	xiaomi,mir3p)
		nand_do_upgrade "$1"
		;;
	linksys,ea8100)
		platform_do_upgrade_linksys "$ARGV"
		;;
	tplink,archer-c50-v4)
		MTD_ARGS="-t romfile"
		default_do_upgrade "$1"
		;;
	ubiquiti,edgerouterx|\
	ubiquiti,edgerouterx-sfp)
		platform_upgrade_ubnt_erx "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
