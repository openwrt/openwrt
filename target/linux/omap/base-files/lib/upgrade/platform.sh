platform_check_image() {
	local board=$(board_name)

	case "$board" in
	*)
		legacy_sdcard_check_image "$@"
		;;
	esac
}

platform_copy_config() {
	local board=$(board_name)

	case "$board" in
	*)
		legacy_sdcard_copy_config
		;;
	esac
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	*)
		legacy_sdcard_do_upgrade "$@"
		;;
	esac
}
