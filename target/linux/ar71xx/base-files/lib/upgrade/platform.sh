#
# Copyright (C) 2009 OpenWrt.org
#

. /lib/ar71xx.sh

PART_NAME=firmware
RAMFS_COPY_DATA=/lib/ar71xx.sh

platform_check_image() {
	local board=$(ar71xx_board_name)
	local magic="$(get_magic_word "$1")"

	[ "$ARGC" -gt 1 ] && return 1

	case "$board" in
	ap83 | mzk-w04nu | mzk-w300nh | tew-632brp | wrt-400n)
		[ "$magic" != "2705" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	tl-wr741nd | tl-wr941nd)
		[ "$magic" != "0100" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	wrt160nl)
		[ "$magic" != "4e4c" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	esac

	echo "Sysupgrade is not yet supported on $board."
	return 1
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
