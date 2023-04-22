platform_check_image() {
	local board=$(board_name)

	case "$board" in
	centurysystems,ma-e350-n-nand)
		nand_do_platform_check "$board" "$@"
		;;
	*)
		legacy_sdcard_check_image "$@"
		;;
	esac
}

platform_copy_config() {
	local board=$(board_name)

	case "$board" in
	centurysystems,ma-e350-n-nand)
		;;
	*)
		legacy_sdcard_copy_config
		;;
	esac
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	centurysystems,ma-e350-n-nand)
		CI_UBIPART="UBI"
		CI_KERNPART="kernel.0"
		nand_do_upgrade "$@"
		;;
	*)
		legacy_sdcard_do_upgrade "$@"
		;;
	esac
}
