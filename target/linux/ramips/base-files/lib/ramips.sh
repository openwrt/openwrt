#!/bin/sh
#
# Copyright (C) 2010-2013 OpenWrt.org
#

RAMIPS_BOARD_NAME=
RAMIPS_MODEL=

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
	*"Ai-BR100")
		name="ai-br100"
		;;
	*"Air3GII")
		name="air3gii"
		;;
	*"ALL0239-3G")
		name="all0239-3g"
		;;
	*"ALL0256N")
		name="all0256n"
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
	*"ASL26555")
		name="asl26555"
		;;
	*"ATP-52B")
		name="atp-52b"
		;;
	*"AWAPN2403")
		name="awapn2403"
		;;
	*"AWM002 EVB")
		name="awm002-evb"
		;;
	*"AWM003 EVB")
		name="awm003-evb"
		;;
	*"BC2")
		name="bc2"
		;;
	*"BR-6425")
		name="br-6425"
		;;
	*"BR-6475nD")
		name="br-6475nd"
		;;
	*"Broadway")
		name="broadway"
		;;
	*"C20i")
		name="c20i"
		;;
	*"Carambola")
		name="carambola"
		;;
	*"CF-WR800N")
		name="cf-wr800n"
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
	*"DB-WRT01")
		name="db-wrt01"
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
	*"DIR-600 B2")
		name="dir-600-b2"
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
	*"DIR-810L")
		name="dir-810l"
		;;
	*"DIR-860L B1")
		name="dir-860l-b1"
		;;
	*"Dovado Tiny AC")
		name="tiny-ac"
		;;
	*"E1700")
		name="e1700"
		;;
	*"ESR-9753")
		name="esr-9753"
		;;
	*"EX2700")
		name="ex2700";
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
	*"FireWRT")
		name="firewrt"
		;;
	*"Fonera 2.0N")
		name="fonera20n"
		;;
	*"FreeStation5")
		name="freestation5"
		;;
	*"GL-MT300A")
		name="gl-mt300a"
		;;
	*"GL-MT300N")
		name="gl-mt300n"
		;;
	*"GL-MT750")
		name="gl-mt750"
		;;
	*"HC5661")
		name="hc5661"
		;;
	*"HC5761")
		name="hc5761"
		;;
	*"HC5861")
		name="hc5861"
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
	*"M4")
		name="m4"
		;;
	*"MediaTek LinkIt Smart 7688")
		linkit="$(dd bs=1 skip=1024 count=12 if=/dev/mtd2 2> /dev/null)"
		if [ "${linkit}" = "LINKITS7688D" ]; then
			name="linkits7688d"
			RAMIPS_MODEL="${machine} DUO"
		else
			name="linkits7688"
		fi
		;;
	*"Memory 2 Move")
		name="m2m"
		;;
	*"MicroWRT")
		name="microwrt"
		;;
	*"MiniEMBPlug")
		name="miniembplug"
		;;
	*"MiniEMBWiFi")
		name="miniembwifi"
		;;
	*"MiWiFi Mini")
		name="miwifi-mini"
		;;
	*"MLW221")
		name="mlw221"
		;;
	*"MLWG2")
		name="mlwg2"
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
	*"MT7620a + MT7530 evaluation"*)
		name="mt7620a_mt7530"
		;;
	*"MT7620a V22SG"*)
		name="mt7620a_v22sg"
		;;
	*"MT7621 evaluation"*)
		name="mt7621"
		;;
	*"MT7628AN evaluation"*)
		name="mt7628"
		;;
	*"MT7688 evaluation"*)
		name="mt7688"
		;;
	*"MZK-750DHP")
		name="mzk-750dhp"
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
	*"NA930")
		name="na930"
		;;
	*"NBG-419N")
		name="nbg-419n"
		;;
	*"NCS601W")
		name="ncs601w"
		;;
	*"NW718")
		name="nw718"
		;;
	*"OY-0001")
		name="oy-0001"
		;;
	*"PBR-M1")
		name="pbr-m1"
		;;
	*"PSG1208")
		name="psg1208"
		;;
	*"PSR-680W"*)
		name="psr-680w"
		;;
	*"PWH2004")
		name="pwh2004"
		;;
	*"PX-4885")
		name="px-4885"
		;;
	*"Q7")
		name="zte-q7"
		;;
	*"RE6500")
		name="re6500"
		;;
	*"RN502J")
		name="xdxrn502j"
		;;
	*"RP-N53")
		name="rp-n53"
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
	*"RT-N14U")
		name="rt-n14u"
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
	*"SAP-G3200U3")
		name="sap-g3200u3"
		;;
	*"SL-R7205"*)
		name="sl-r7205"
		;;
	*"TEW-691GR")
		name="tew-691gr"
		;;
	*"TEW-692GR")
		name="tew-692gr"
		;;
	*"UBNT-ERX")
		name="ubnt-erx"
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
	*"VoCore")
		name="vocore"
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
	*"WF-2881")
		name="wf-2881"
		;;
	*"WHR-1166D")
		name="whr-1166d"
		;;
	*"WHR-300HP2")
		name="whr-300hp2"
		;;
	*"WHR-600D")
		name="whr-600d"
		;;
	*"WHR-G300N")
		name="whr-g300n"
		;;
	*"WiTi")
                name="witi"
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
	*"WL-341 v3")
		name="wl-341v3"
		;;
	*"WL-351 v1 002")
		name="wl-351"
		;;
	*"WLI-TX4-AG300N")
		name="wli-tx4-ag300n"
		;;
	*"WMR-300")
		name="wmr-300"
		;;
	*"WNCE2001")
		name="wnce2001"
		;;
	*"WR512-3GN-like"*)
		name="wr512-3gn"
		;;
	*"WR6202")
		name="wr6202"
		;;
	*"WRTNODE")
		name="wrtnode"
		;;
	*"WRTnode2R")
		name="wrtnode2r"
		;;
	*"WRTnode2P")
		name="wrtnode2p"
		;;
	*"WSR-1166DHP")
		name="wsr-1166"
		;;
	*"WSR-600DHP")
		name="wsr-600"
		;;
	*"WT1520")
		name="wt1520"
		;;
	*"WT3020")
		name="wt3020"
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
	*"Y1")
		name="y1"
		;;
	*"Y1S")
		name="y1s"
		;;
	*"ZBT-WA05")
		name="zbt-wa05"
		;;
	*"ZBT-WE826")
		name="zbt-we826"
		;;
	*"ZBT-WG2626")
		name="zbt-wg2626"
		;;
	*"ZBT-WR8305RT")
		name="zbt-wr8305rt"
		;;
	*"YK1")
		name="youku-yk1"
		;;
	*)
		name="generic"
		;;
	esac

	[ -z "$RAMIPS_BOARD_NAME" ] && RAMIPS_BOARD_NAME="$name"
	[ -z "$RAMIPS_MODEL" ] && RAMIPS_MODEL="$machine"

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	echo "$RAMIPS_BOARD_NAME" > /tmp/sysinfo/board_name
	echo "$RAMIPS_MODEL" > /tmp/sysinfo/model
}

ramips_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)
	[ -z "$name" ] && name="unknown"

	echo "$name"
}
