#
# Copyright (C) 2014 OpenWrt.org
#

. /lib/mvebu.sh

RAMFS_COPY_DATA=/lib/mvebu.sh

platform_check_image() {
	local board=$(mvebu_board_name)
	local magic_long="$(get_magic_long "$1")"

	[ "$#" -gt 1 ] && return 1

	case "$board" in
	armada-xp-mamba )
		[ "$magic_long" != "27051956" -a "$magic_long" != "73797375" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0;
		;;
	esac

	echo "Sysupgrade is not yet supported on $board."
	return 1
}

platform_do_upgrade() {
	local board=$(mvebu_board_name)

	case "$board" in
	armada-xp-mamba)
		platform_do_upgrade_linksys "$ARGV"
		;;
	*)
		default_do_upgrade "$ARGV"
		;;
	esac
}

disable_watchdog() {
	killall watchdog
	( ps | grep -v 'grep' | grep '/dev/watchdog' ) && {
		echo 'Could not disable watchdog'
		return 1
	}
}

append sysupgrade_pre_upgrade disable_watchdog
