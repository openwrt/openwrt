#
# Copyright (C) 2014 OpenWrt.org
#

. /lib/oxnas.sh

RAMFS_COPY_DATA=/lib/oxnas.sh

platform_check_image() {
	local board=$(oxnas_board_name)

	[ "$ARGC" -gt 1 ] && return 1

	nand_do_platform_check $board $1
	return $?
}

platform_do_upgrade() {
	if [ $$ -ne 1 ]; then
		echo "not PID 1, upgrade aborted."
		return 1;
	fi
	platform_do_upgrade_phase2 "$1" "$CONF_TAR" "$SAVE_CONFIG"
}


disable_watchdog() {
	killall watchdog
	( ps | grep -v 'grep' | grep '/dev/watchdog' ) && {
		echo 'Could not disable watchdog'
		return 1
	}
}

append sysupgrade_pre_upgrade disable_watchdog
