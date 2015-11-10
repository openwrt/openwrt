. /lib/kirkwood.sh

platform_check_image() {
	[ "$#" -gt 1 ] && return 1
	local board="$(kirkwood_board_name)"
	local magic="$(get_magic_long "$1")"

	case "$board" in
	"linksys-audi"|\
	"linksys-viper")
		[ "$magic" != "27051956" -a "$magic" != "73797375" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	esac

	echo "Sysupgrade is not yet supported on $board."
	return 1
}

platform_do_upgrade() {
	local board="$(kirkwood_board_name)"

	case "$board" in
	"linksys-audi"|\
	"linksys-viper")
		platform_do_upgrade_linksys "$ARGV"
		;;
	esac
}
