#
# Copyright (C) 2010 OpenWrt.org
#

. /lib/ramips.sh

PART_NAME=firmware
RAMFS_COPY_DATA=/lib/ramips.sh

platform_check_image() {
	local board=$(ramips_board_name)
	local magic="$(get_magic_long "$1")"

	[ "$#" -gt 1 ] && return 1

	case "$board" in
	3g150b|\
	3g300m|\
	3g-6200n|\
	3g-6200nl|\
	a5-v11|\
	ai-br100|\
	air3gii|\
	all0239-3g|\
	all0256n|\
	all5002|\
	all5003|\
	ar725w|\
	asl26555|\
	awapn2403|\
	awm002-evb|\
	awm003-evb|\
	bc2|\
	broadway|\
	carambola|\
	cf-wr800n|\
	d105|\
	dap-1350|\
	db-wrt01|\
	dcs-930|\
	dcs-930l-b1|\
	dir-300-b1|\
	dir-300-b7|\
	dir-320-b1|\
	dir-600-b1|\
	dir-600-b2|\
	dir-615-d|\
	dir-615-h1|\
	dir-620-a1|\
	dir-620-d1|\
	dir-810l|\
	e1700|\
	esr-9753|\
	ex2700|\
	f7c027|\
	firewrt|\
	fonera20n|\
	freestation5|\
	gl-mt300a|\
	gl-mt300n|\
	gl-mt750|\
	hc5*61|\
	hg255d|\
	hlk-rm04|\
	hpm|\
	ht-tm02|\
	hw550-3g|\
	ip2202|\
	jhr-n805r|\
	jhr-n825r|\
	jhr-n926r|\
	linkits7688|\
	linkits7688d|\
	m2m|\
	m3|\
	m4|\
	microwrt|\
	miniembplug|\
	miniembwifi|\
	miwifi-mini|\
	mlw221|\
	mlwg2|\
	mofi3500-3gn|\
	mpr-a1|\
	mpr-a2|\
	mr-102n|\
	mt7628|\
	mzk-dp150n|\
	mzk-w300nh2|\
	mzk-wdpr|\
	nbg-419n|\
	nw718|\
	oy-0001|\
	pbr-m1|\
	psg1208|\
	psr-680w|\
	px-4885|\
	re6500|\
	rp-n53|\
	rt5350f-olinuxino|\
	rt5350f-olinuxino-evb|\
	rt-g32-b1|\
	rt-n10-plus|\
	rt-n13u|\
	rt-n14u|\
	rt-n15|\
	rt-n56u|\
	rut5xx|\
	sap-g3200u3|\
	sl-r7205|\
	tew-691gr|\
	tew-692gr|\
	tiny-ac|\
	ur-326n4g|\
	ur-336un|\
	v22rw-2x2|\
	vocore|\
	w150m|\
	w306r-v20|\
	w502u|\
	wf-2881|\
	whr-1166d|\
	whr-300hp2|\
	whr-600d|\
	whr-g300n|\
	witi|\
	wizfi630a|\
	wl-330n|\
	wl-330n3g|\
	wl-341v3|\
	wl-351|\
	wli-tx4-ag300n|\
	wmr-300|\
	wnce2001|\
	wr512-3gn|\
	wr6202|\
	wrtnode|\
	wrtnode2r |\
	wrtnode2p |\
	wsr-600|\
	wt1520|\
	wt3020|\
	wzr-agl300nh|\
	x5|\
	x8|\
	y1|\
	y1s|\
	zbt-wa05|\
	zbt-we826|\
	zbt-wg2626|\
	zbt-wr8305rt|\
	zte-q7|\
	youku-yk1)
		[ "$magic" != "27051956" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	ar670w)
		[ "$magic" != "6d000080" ] && {
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
	c20i)
		[ "$magic" != "03000000" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	cy-swr1100|\
	dir-610-a1|\
	dir-645|\
	dir-860l-b1)
		[ "$magic" != "5ea3a417" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	wsr-1166)
		[ "$magic" != "48445230" ] && {
			echo "Invalid image type."
			return 1
		}
		return 0
		;;
	ubnt-erx)
		nand_do_platform_check "$board" "$1"
		return $?;
		;;
	esac

	echo "Sysupgrade is not yet supported on $board."
	return 1
}

platform_nand_pre_upgrade() {
	local board=$(ramips_board_name)

	case "$board" in
	ubnt-erx)
		platform_upgrade_ubnt_erx "$ARGV"
		;;
	esac
}

platform_pre_upgrade() {
	local board=$(ramips_board_name)

	case "$board" in
    	ubnt-erx)
		nand_do_upgrade "$ARGV"
		;;
	esac
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

blink_led() {
	. /etc/diag.sh; set_state upgrade
}

append sysupgrade_pre_upgrade disable_watchdog
append sysupgrade_pre_upgrade blink_led
