#!/bin/sh
#
# Copyright (C) 2009 OpenWrt.org
#

ar71xx_board_name() {
	local machine
	local name

	machine=$(awk 'BEGIN{FS="[ \t]+:[ \t]"} /machine/ {print $2}' /proc/cpuinfo)

	case "$machine" in
	*AP81)
		name="ap81"
		;;
	*AP83)
		name="ap83"
		;;
	*AW-NR580)
		name="aw-nr580"
		;;
	*"DIR-600 rev. A1")
		name="dir-600-a1"
		;;
	*"DIR-825 rev. B1")
		name="dir-825-b1"
		;;
	*"Bullet M")
		name="bullet-m"
		;;
	*"Nanostation M")
		name="nanostation-m"
		;;
	*LS-SR71)
		name="ls-sr71"
		;;
	*MZK-W04NU)
		name="mzk-w04nu"
		;;
	*MZK-W300NH)
		name="mzk-w300nh"
		;;
	*PB42)
		name="pb42"
		;;
	*PB44)
		name="pb44"
		;;
	*RB-411)
		name="rb-411"
		;;
	*RB-433)
		name="rb-433"
		;;
	*RB-450)
		name="rb-450"
		;;
	*RB-493)
		name="rb-493"
		;;
	*"Rocket M")
		name="rocket-m"
		;;
	*RouterStation)
		name="routerstation"
		;;
	*"RouterStation Pro")
		name="routerstation-pro"
		;;
	*TEW-632BRP)
		name="tew-632brp"
		;;
	*TL-WR1043ND)
		name="tl-wr1043nd"
		;;
	*"DIR-615 rev. C1")
		name="dir-615-c1"
		;;
	*TL-WR741ND)
		name="tl-wr741nd"
		;;
	*"TL-WR841N v1")
		name="tl-wr841n-v1"
		;;
	*TL-WR941ND)
		name="tl-wr941nd"
		;;
	*WP543)
		name="wp543"
		;;
	*WNDR3700)
		name="wndr3700"
		;;
	*WNR2000)
		name="wnr2000"
		;;
	*WRT160NL)
		name="wrt160nl"
		;;
	*WRT400N)
		name="wrt400n"
		;;
	*WZR-HP-G300NH)
		name="wzr-hp-g300nh"
		;;
	*)
		name="generic"
		;;
	esac

	echo $name
}
