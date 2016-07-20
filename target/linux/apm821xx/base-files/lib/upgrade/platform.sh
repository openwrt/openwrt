#!/bin/sh

. /lib/apm821xx.sh

PART_NAME=firmware
RAMFS_COPY_DATA=/lib/apm821xx.sh

platform_check_image() {
	local board=$(apm821xx_board_name)

	[ "$#" -gt 1 ] && return 1

	case "$board" in
	*)
		;;
	esac

	echo "Sysupgrade is not yet supported on $board."
	return 1
}

platform_pre_upgrade() {
	local board=$(apm821xx_board_name)

	case "$board" in
	*)
		;;
	esac
}

platform_do_upgrade() {
	local board=$(apm821xx_board_name)

	case "$board" in
	*)
		default_do_upgrade "$ARGV"
		;;
	esac
}

platform_copy_config() {
	local board=$(apm821xx_board_name)

	case "$board" in
	*)
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
