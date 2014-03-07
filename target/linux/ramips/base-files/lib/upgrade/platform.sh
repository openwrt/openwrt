#
# Copyright (C) 2010 OpenWrt.org
#

. /lib/ramips.sh

PART_NAME=firmware
RAMFS_COPY_DATA=/lib/ramips.sh

platform_check_image() {
	local board=$(ramips_board_name)
	local magic="$(get_magic_long "$1")"

	[ "$ARGC" -gt 1 ] && return 1

	case "$board" in
	3g-6200n | \
	3g-6200nl | \
	3g300m | \
	w150m | \
	air3gii | \
	all0239-3g | \
	all0256n | \
	all5002 | \
	all5003 | \
	asl26555 | \
	awm002-evb | \
	bc2 | \
	broadway | \
	carambola | \
	d105 | \
	dcs-930 | \
	dir-300-b1 | \
	dir-300-b7 | \
	dir-320-b1 | \
	dir-600-b1 | \
	dir-600-b2 | \
	dir-615-h1 | \
	dir-615-d | \
	dir-620-a1 | \
	dir-620-d1 | \
	dir-810l | \
	dap-1350 | \
	esr-9753 | \
	f7c027 | \
	fonera20n | \
	rt-n13u | \
	freestation5 | \
	hw550-3g | \
	hg255d | \
	hlk-rm04 | \
	ip2202 | \
	m3 | \
	m4 | \
	mofi3500-3gn | \
	mpr-a1 | \
	mpr-a2 | \
	mzk-w300nh2 | \
	nbg-419n | \
	nw718 | \
	omni-emb | \
	omni-emb-hpm | \
	psr-680w | \
	rp-n53 | \
	rt-g32-b1 | \
	rt-n10-plus | \
	rt-n14u | \
	rt-n15 | \
	rt-n56u | \
	sl-r7205 | \
	tew-691gr | \
	tew-692gr | \
	w306r-v20 |\
	w502u |\
	wr6202 |\
	v22rw-2x2 | \
	wl341v3 | \
	wl-330n | \
	wl-330n3g | \
	wl-351 | \
	wnce2001 | \
	wli-tx4-ag300n | \
	whr-g300n |\
	ur-326n4g |\
	ur-336un |\
	wr512-3gn |\
	wmr300 |\
	wrtnode |\
	x5 |\
	x8)
		[ "$magic" != "27051956" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	dir-610-a1 |\
	dir-645)
		[ "$magic" != "5ea3a417" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	br-6475nd)
		[ "$magic" != "43535953" ] && {
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
