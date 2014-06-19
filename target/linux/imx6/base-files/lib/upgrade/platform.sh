#
# Copyright (C) 2010 OpenWrt.org
#

. /lib/functions/imx6.sh

platform_check_image() {
	local board=$(imx6_board_name)

	case "$board" in
	gw54xx)
		nand_do_platform_check $board $1
		return $?;
		;;
	esac

	echo "Sysupgrade is not yet supported on $board."
	return 1
}
