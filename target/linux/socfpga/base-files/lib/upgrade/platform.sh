#
# Copyright (C) 2014 OpenWrt.org
#

. /lib/socfpga.sh

RAMFS_COPY_BIN="/bin/mkdir /bin/touch /bin/mknod"
RAMFS_COPY_DATA=/lib/socfpga.sh

platform_check_image() {
	local board=$(socfpga_board_name)

	[ "$#" -gt 1 ] && return 1

	case "$board" in
	"socfpga-sockit")
		platform_do_check_sockit "$ARGV"
		return 0;
		;;
	esac

	echo "Sysupgrade is not yet supported on $board."
	return 1
}

platform_do_upgrade() {
	local board=$(socfpga_board_name)

	case "$board" in
	"socfpga-sockit")
		platform_do_upgrade_sockit "$ARGV"
		;;
	*)
		default_do_upgrade "$ARGV"
		;;
	esac
}
