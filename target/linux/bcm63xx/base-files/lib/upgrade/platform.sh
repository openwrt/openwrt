PART_NAME=linux
REQUIRE_IMAGE_METADATA=0

platform_check_image() {
	[ "$#" -gt 1 ] && return 1

	case "$(board_name)" in
		comtrend,vg-8050|\
		comtrend,vr-3032u|\
		huawei,hg253s-v2|\
		netgear,dgnd3700-v2|\
		sercomm,ad1018|\
		sercomm,h500-s-lowi|\
		sercomm,h500-s-vfes)
			# NAND sysupgrade
			return 0
			;;
	esac

	case "$(get_magic_word "$1")" in
		3600|3700|3800)
			# CFE tag versions
			return 0
			;;
		*)
			echo "Invalid image type. Please use only .bin files"
			return 1
			;;
	esac
}

platform_do_upgrade() {
	case "$(board_name)" in
		comtrend,vg-8050|\
		comtrend,vr-3032u|\
		huawei,hg253s-v2|\
		netgear,dgnd3700-v2)
			REQUIRE_IMAGE_METADATA=1
			CI_JFFS2_CLEAN_MARKERS=1
			nand_do_upgrade "$1"
			;;
		sercomm,ad1018|\
		sercomm,h500-s-lowi|\
		sercomm,h500-s-vfes)
			REQUIRE_IMAGE_METADATA=1
			nand_do_upgrade "$1"
			;;
		*)
			default_do_upgrade "$1"
			;;
	esac
}
