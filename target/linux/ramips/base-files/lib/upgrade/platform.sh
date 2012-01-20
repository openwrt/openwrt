#
# Copyright (C) 2010 OpenWrt.org
#

. /lib/ramips.sh

PART_NAME=firmware
RAMFS_COPY_DATA=/lib/ramips.sh

platform_check_image() {
	local board=$(ramips_board_name)
	local magic="$(get_magic_word "$1")"

	[ "$ARGC" -gt 1 ] && return 1

	case "$board" in
	bc2 | \
	dir-300-b1 | \
	dir-600-b1 | \
	dir-600-b2 | \
	esr-9753 | \
	fonera20n | \
	hw550-3g | \
	mofi3500-3gn | \
	nbg-419n | \
	nw718 | \
	omni-emb | \
	rt-g32-b1 | \
	rt-n15 | \
	w502u |\
	v22rw-2x2 | \
	wli-tx4-ag300n | \
	whr-g300n)
		[ "$magic" != "2705" ] && {
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
	local board=$(ramips_board_name)

	case "$board" in
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
