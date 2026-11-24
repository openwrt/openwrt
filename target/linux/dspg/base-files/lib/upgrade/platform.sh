platform_check_image() {
	local board=$(board_name)

	case "$board" in
	snom,d735)
		nand_do_platform_check "$board" "$1"
		return $?
		;;
	esac

	return 1
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	snom,d735)
		nand_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
