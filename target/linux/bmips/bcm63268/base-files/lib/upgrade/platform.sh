# SPDX-License-Identifier: GPL-2.0-or-later

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	return 0
}

platform_do_upgrade() {
	case "$(board_name)" in
	comtrend,vg-8050 |\
	comtrend,vr-3032u)
		CI_JFFS2_CLEAN_MARKERS=1
		nand_do_upgrade "$1"
		;;
	sercomm,h500-s-lowi |\
	sercomm,h500-s-vfes |\
	sercomm,shg2500)
		nand_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
