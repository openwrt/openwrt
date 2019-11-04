#
# Copyright (C) 2011 OpenWrt.org
#

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	return 0
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	glinet,gl-ar300m-nand)
		default_do_upgrade "$1"
		;;
	*)
		nand_do_upgrade "$1"
		;;
	esac
}
