# SPDX-License-Identifier: GPL-2.0-or-later

. /lib/functions.sh
. /lib/upgrade/nand.sh

PART_NAME=image2

platform_check_image()
{
	case "$(board_name)" in
	netgear,cg3000-2staus)
		return 0
		;;
	esac

	return 1
}

platform_do_upgrade()
{
	case "$(board_name)" in
	netgear,cg3000-2staus)
		CI_UBIPART="$PART_NAME"
		nand_do_upgrade "$1"
		;;
	esac
}
