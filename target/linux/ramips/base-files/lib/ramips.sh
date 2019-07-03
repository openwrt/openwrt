#!/bin/sh
#
# Copyright (C) 2010-2013 OpenWrt.org
#

ramips_board_detect() {
	local machine
	local name

	machine=$(awk 'BEGIN{FS="[ \t]+:[ \t]"} /machine/ {print $2}' /proc/cpuinfo)

	case "$machine" in
	*"3G150B")
		name="3g150b"
		;;
	*"3G300M")
		name="3g300m"
		;;
	*"3g-6200n")
		name="3g-6200n"
		;;
	*"3g-6200nl")
		name="3g-6200nl"
		;;
	*"A5-V11")
		name="a5-v11"
		;;
	*"Air3GII")
		name="air3gii"
		;;
	*"ALL0256N (4M)")
		name="all0256n-4M"
		;;
	*"ALL0256N (8M)")
		name="all0256n-8M"
		;;
	*"ALL5002")
		name="all5002"
		;;
	*"ALL5003")
		name="all5003"
		;;
	*"AR670W")
		name="ar670w"
		;;
	*"AR725W")
		name="ar725w"
		;;
	*"ASL26555 (8M)")
		name="asl26555-8M"
		;;
	*"ASL26555 (16M)")
		name="asl26555-16M"
		;;
	*"ATP-52B")
		name="atp-52b"
		;;
	*"AWAPN2403")
		name="awapn2403"
		;;
	*"AWM002 EVB (4M)")
		name="awm002-evb-4M"
		;;
	*"AWM002 EVB (8M)")
		name="awm002-evb-8M"
		;;
	*"BC2")
		name="bc2"
		;;
	*"BR-6475nD")
		name="br-6475nd"
		;;
	*"Broadway")
		name="broadway"
		;;
	*"Carambola")
		name="carambola"
		;;
	*"CY-SWR1100")
		name="cy-swr1100"
		;;
	*"D105")
		name="d105"
		;;
	*"DAP-1350")
		name="dap-1350"
		;;
	*"DCS-930")
		name="dcs-930"
		;;
	*"DCS-930L B1")
		name="dcs-930l-b1"
		;;
	*"DIR-300 B1")
		name="dir-300-b1"
		;;
	*"DIR-300 B7")
		name="dir-300-b7"
		;;
	*"DIR-320 B1")
		name="dir-320-b1"
		;;
	*"DIR-600 B1")
		name="dir-600-b1"
		;;
	*"DIR-610 A1")
		name="dir-610-a1"
		;;
	*"DIR-615 D")
		name="dir-615-d"
		;;
	*"DIR-615 H1")
		name="dir-615-h1"
		;;
	*"DIR-620 A1")
		name="dir-620-a1"
		;;
	*"DIR-620 D1")
		name="dir-620-d1"
		;;
	*"DIR-645")
		name="dir-645"
		;;
	*"DWR-512 B")
		name="dwr-512-b"
		;;
	*"ESR-9753")
		name="esr-9753"
		;;
	*"F5D8235 v1")
		name="f5d8235-v1"
		;;
	*"F5D8235 v2")
		name="f5d8235-v2"
		;;
	*"F7C027")
		name="f7c027"
		;;
	*"Fonera 2.0N")
		name="fonera20n"
		;;
	*"FreeStation5")
		name="freestation5"
		;;
	*"HG255D")
		name="hg255d"
		;;
	*"HLK-RM04")
		name="hlk-rm04"
		;;
	*"HPM")
		name="hpm"
		;;
	*"HT-TM02")
		name="ht-tm02"
		;;
	*"HW550-3G")
		name="hw550-3g"
		;;
	*"IP2202")
		name="ip2202"
		;;
	*"JHR-N805R")
		name="jhr-n805r"
		;;
	*"JHR-N825R")
		name="jhr-n825r"
		;;
	*"JHR-N926R")
		name="jhr-n926r"
		;;
	*"M3")
		name="m3"
		;;
	*"M4 (4M)")
		name="m4-4M"
		;;
	*"M4 (8M)")
		name="m4-8M"
		;;
	*"Memory 2 Move")
		name="m2m"
		;;
	*"MiniEMBPlug")
		name="miniembplug"
		;;
	*"MiniEMBWiFi")
		name="miniembwifi"
		;;
	*"MOFI3500-3GN")
		name="mofi3500-3gn"
		;;
	*"MPR-A1")
		name="mpr-a1"
		;;
	*"MPR-A2")
		name="mpr-a2"
		;;
	*"MR-102N")
		name="mr-102n"
		;;
	*"MT7688 evaluation"*)
		name="mt7688"
		;;
	*"MZK-DP150N")
		name="mzk-dp150n"
		;;
	*"MZK-W300NH2"*)
		name="mzk-w300nh2"
		;;
	*"MZK-WDPR"*)
		name="mzk-wdpr"
		;;
	*"NBG-419N")
		name="nbg-419n"
		;;
	*"NBG-419N v2")
		name="nbg-419n2"
		;;
	*"NCS601W")
		name="ncs601w"
		;;
	*"NixcoreX1 (8M)")
		name="nixcore-x1-8M"
		;;
	*"NixcoreX1 (16M)")
		name="nixcore-x1-16M"
		;;
	*"NW718")
		name="nw718"
		;;
	*"PSR-680W"*)
		name="psr-680w"
		;;
	*"PWH2004")
		name="pwh2004"
		;;
	*"PX-4885 (4M)")
		name="px-4885-4M"
		;;
	*"PX-4885 (8M)")
		name="px-4885-8M"
		;;
	*"RN502J")
		name="xdxrn502j"
		;;
	*"RT5350F-OLinuXino")
		name="rt5350f-olinuxino"
		;;
	*"RT5350F-OLinuXino-EVB")
		name="rt5350f-olinuxino-evb"
		;;
	*"RT-G32 B1")
		name="rt-g32-b1"
		;;
	*"RT-N10+")
		name="rt-n10-plus"
		;;
	*"RT-N13U")
		name="rt-n13u"
		;;
	*"RT-N15")
		name="rt-n15"
		;;
	*"RT-N56U")
		name="rt-n56u"
		;;
	*"RUT5XX")
		name="rut5xx"
		;;
	*"SL-R7205"*)
		name="sl-r7205"
		;;
	*"TEW-638APB v2")
		name="tew-638apb-v2"
		;;
	*"TEW-691GR")
		name="tew-691gr"
		;;
	*"TEW-692GR")
		name="tew-692gr"
		;;
	*"TEW-714TRU")
		name="tew-714tru"
		;;
	*"UR-326N4G")
		name="ur-326n4g"
		;;
	*"UR-336UN")
		name="ur-336un"
		;;
	*"V11ST-FE")
		name="v11st-fe"
		;;
	*"V22RW-2X2")
		name="v22rw-2x2"
		;;
	*"VoCore (8M)")
		name="vocore-8M"
		;;
	*"VoCore (16M)")
		name="vocore-16M"
		;;
	*"W150M")
		name="w150m"
		;;
	*"W306R V2.0")
		name="w306r-v20"
		;;
	*"W502U")
		name="w502u"
		;;
	*"WCR-150GN")
		name="wcr-150gn"
		;;
	*"WHR-G300N")
		name="whr-g300n"
		;;
	*"WIZARD 8800")
		name="wizard8800"
		;;
	*"WizFi630A")
		name="wizfi630a"
		;;
	*"WL-330N")
		name="wl-330n"
		;;
	*"WL-330N3G")
		name="wl-330n3g"
		;;
	*"WL-351 v1 002")
		name="wl-351"
		;;
	*"WLI-TX4-AG300N")
		name="wli-tx4-ag300n"
		;;
	*"WLR-6000")
		name="wlr-6000"
		;;
	*"WMDR-143N")
		name="wmdr-143n"
		;;
	*"WNCE2001")
		name="wnce2001"
		;;
	*"WR512-3GN (4M)")
		name="wr512-3gn-4M"
		;;
	*"WR512-3GN (8M)")
		name="wr512-3gn-8M"
		;;
	*"WR6202")
		name="wr6202"
		;;
	*"WT1520 (4M)")
		name="wt1520-4M"
		;;
	*"WT1520 (8M)")
		name="wt1520-8M"
		;;
	*"WZR-AGL300NH")
		name="wzr-agl300nh"
		;;
	*"X5")
		name="x5"
		;;
	*"X8")
		name="x8"
		;;
	*"ZyXEL Keenetic")
		name="kn"
		;;
	esac

	# use generic board detect if no name is set
	[ -z "$name" ] && return

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	echo "$name" > /tmp/sysinfo/board_name
	echo "$machine" > /tmp/sysinfo/model
}
