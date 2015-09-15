. /lib/functions/lantiq.sh

PART_NAME=firmware

platform_check_image() {
	[ "$#" -gt 1 ] && return 1
	local board=$(lantiq_board_name)

	case "$board" in
		BTHOMEHUBV2B|BTHOMEHUBV3A|BTHOMEHUBV5A|P2812HNUF* )
			nand_do_platform_check $board $1
			return $?;
			;;
	esac

	case "$(get_magic_word "$1")" in
		# uImage
		2705) return 0;;
		# tplink
		0200) return 0;;
		*)
			echo "Invalid image type"
			return 1
		;;
	esac
}

platform_pre_upgrade() {
	local board=$(lantiq_board_name)

	case "$board" in
	BTHOMEHUBV2B|BTHOMEHUBV3A|BTHOMEHUBV5A|P2812HNUF* )
		nand_do_upgrade $1
		;;
	esac
}

# use default for platform_do_upgrade()

disable_watchdog() {
	killall watchdog
	( ps | grep -v 'grep' | grep '/dev/watchdog' ) && {
		echo 'Could not disable watchdog'
		return 1
	}
}
append sysupgrade_pre_upgrade disable_watchdog
