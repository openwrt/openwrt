#
# Copyright (C) 2013 OpenWrt.org
#

PART_NAME=firmware

platform_check_image() {
	local magic="$(get_magic_long "$1")"

	[ "$#" -gt 1 ] && return 1

	[ "$magic" != "27051956" ] && {
		echo "Invalid image type."
		return 1
	}

	return 0
}

disable_watchdog() {
	killall watchdog
	( ps | grep -v 'grep' | grep '/dev/watchdog' ) && {
		echo 'Could not disable watchdog'
		return 1
	}
}

append sysupgrade_pre_upgrade disable_watchdog
