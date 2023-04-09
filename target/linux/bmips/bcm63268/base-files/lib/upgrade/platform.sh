# SPDX-License-Identifier: GPL-2.0-or-later

. /lib/upgrade/cfe-jffs2-nand.sh

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	return 0
}

platform_do_upgrade() {
	case "$(board_name)" in
	comtrend,vr-3032u)
		cfe_jffs2_nand_upgrade "$1"
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
