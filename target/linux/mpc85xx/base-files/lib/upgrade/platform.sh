#
# Copyright (C) 2011 OpenWrt.org
#

REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	local board=$(board_name)

	case "$board" in
	watchguard,firebox-t30)
		# T30 uses MMC, no MTD firmware partition
		return 0
		;;
	esac

	# Default: require firmware MTD for other boards
	PART_NAME=firmware
	return 0
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	hpe,msm460|\
	ocedo,panda|\
	sophos,red-15w-rev1|\
	watchguard,firebox-t10|\
	watchguard,firebox-t15|\
	watchguard,xtm330)
		nand_do_upgrade "$1"
		;;
	watchguard,firebox-t30)
		watchguard_sdcard_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}