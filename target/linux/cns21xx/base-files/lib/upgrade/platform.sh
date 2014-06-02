#
# Copyright (C) 2010 OpenWrt.org
#

. /lib/cns21xx.sh

PART_NAME=firmware
RAMFS_COPY_DATA=/lib/cns21xx.sh

platform_check_image() {
	local board=$(get_board_name)
	local magic="$(get_magic_word "$1")"

	[ "$#" -gt 1 ] && return 1

	case "$board" in
	nsb3ast)
		[ "$magic" != "0b1c" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	ns-k330)
		[ "$magic" != "0c1c" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	esac

	echo "Sysupgrade is not yet supported on $board."
	return 1
}

disable_watchdog() {
	killall watchdog
	( ps | grep -v 'grep' | grep '/dev/watchdog' ) && {
		echo 'Could not disable watchdog'
		return 1
	}
}

append sysupgrade_pre_upgrade disable_watchdog
