#
# Copyright (C) 2011 OpenWrt.org
#

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	case "$(board_name)" in
	nec,ix2106)
		ix2106_check_image "$1"
		return $?
		;;
	esac

	return 0
}

platform_pre_upgrade() {
	case "$(board_name)" in
	nec,ix2106)
		# Bind the input and stripped image checked by platform_check_image.
		# Do not remove the caller's file: -T and user-supplied /tmp paths use it.
		ix2106_prepare_upgrade "$1" || return 1
		;;
	esac
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	nec,ix2106)
		ix2106_do_upgrade "$1"
		;;
	hpe,msm460|\
	ocedo,panda|\
	sophos,red-15w-rev1|\
	watchguard,firebox-t10|\
	watchguard,firebox-t15|\
	watchguard,xtm330)
		nand_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
