platform_check_image() {
	local board=$(board_name)

	case "$board" in
	chinamobile,gs3101|\
	dasan,h660gm-a-airtel|\
	dasan,h660gm-a-generic)
		return 0
		;;
	esac

	return 1
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	chinamobile,gs3101|\
	dasan,h660gm-a-airtel|\
	dasan,h660gm-a-generic)
		CI_KERNPART="tclinux_kernel"
		nand_do_upgrade "$1"
		;;
	esac
}
