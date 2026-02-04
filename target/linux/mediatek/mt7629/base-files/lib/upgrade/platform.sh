PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	return 0
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	iptime,a6004mx)
		nand_do_upgrade "$1"
		;;
	linksys,ea7500-v3)
		fw_setenv boot_part 1
		fw_setenv bootimage 1
		nand_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
