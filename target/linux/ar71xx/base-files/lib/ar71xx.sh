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
	*AP96)
		name="ap96"
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
	*EAP7660D)
		name="eap7660d"
		;;
	*JA76PF)
		name="ja76pf"
		;;
	*"Bullet M")
		name="bullet-m"
		;;
	*"Nanostation M")
		name="nanostation-m"
		;;
	*JWAP003)
		name="jwap003"
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
	*"NBG460N/550N/550NH")
		name="nbg460n_550n_550nh"
		;;
	*PB42)
		name="pb42"
		;;
	*PB44)
		name="pb44"
		;;
	*"RouterBOARD 411/A/AH")
		name="rb-411"
		;;
	*"RouterBOARD 411U")
		name="rb-411u"
		;;
	*"RouterBOARD 433/AH")
		name="rb-433"
		;;
	*"RouterBOARD 433UAH")
		name="rb-433u"
		;;
	*"RouterBOARD 450")
		name="rb-450"
		;;
	*"RouterBOARD 450G")
		name="rb-450g"
		;;
	*"RouterBOARD 493/AH")
		name="rb-493"
		;;
	*"RouterBOARD 750")
		name="rb-750"
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
	*TL-MR3220)
		name="tl-mr3220"
		;;
	*TL-MR3420)
		name="tl-mr3420"
		;;
	*TL-WA901ND)
		name="tl-wa901nd"
		;;
	*"TL-WA901ND v2")
		name="tl-wa901nd-v2"
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
	*UniFi)
		name="unifi"
		;;
	*WP543)
		name="wp543"
		;;
	*WNDR3700)
		name="wndr3700"
		;;
	*WNDR3700v2)
		name="wndr3700v2"
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
	*ZCN-1523H-2)
		name="zcn-1523h-2"
		;;
	*ZCN-1523H-5)
		name="zcn-1523h-5"
		;;
	*)
		name="generic"
		;;
	esac

	echo $name
}
