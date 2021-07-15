REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	return 0
}

platform_do_upgrade() {
	case "$(board_name)" in
	meraki,mx64|\
	meraki,mx64a0|\
	meraki,mx65)
		CI_KERNPART="bootkernel1"
		nand_do_upgrade "$1"
		;;
	*)
		default_do_upgrade "$1"
		;;
	esac
}
