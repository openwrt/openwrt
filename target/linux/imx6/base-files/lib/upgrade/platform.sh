#
# Copyright (C) 2010-2015 OpenWrt.org
#

. /lib/imx6.sh

platform_check_image() {
	local board=$(imx6_board_name)

	case "$board" in
	*gw5*)
		nand_do_platform_check $board $1
		return $?;
		;;
	esac

	echo "Sysupgrade is not yet supported on $board."
	return 1
}

platform_pre_upgrade() {
	local board=$(imx6_board_name)

	case "$board" in
	*gw5*)
		nand_do_upgrade "$1"
		;;
	esac
}
