platform_check_image() {
	local board=$(board_name)

	case "$board" in
	chinamobile,gs3101)
		return 0
		;;
	esac

	return 1
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	chinamobile,gs3101)
		CI_KERNPART="tclinux_kernel"
		nand_do_upgrade "$1"
		;;
	esac
}
