# SPDX-License-Identifier: GPL-2.0-or-later

. /lib/upgrade/cfe-jffs2-nand.sh

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	return 0
}

platform_do_upgrade() {
	case "$(board_name)" in
	huawei,hg253s-v2 |\
	netgear,dgnd3700-v2)
		cfe_jffs2_nand_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
