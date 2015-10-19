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
	*"7Links PX-4885")
		name="px4885"
		;;
	*"8devices Carambola")
		name="carambola"
		;;
	*"Edimax 3g-6200n")
		name="3g-6200n"
		;;
	*"Edimax 3g-6200nl")
		name="3g-6200nl"
		;;
	*"A5-V11")
		name="a5-v11"
		;;
	*"Aigale Ai-BR100")
		name="ai-br100"
		;;
	*"Airlink101 AR670W")
		name="ar670w"
		;;
	*"Airlink101 AR725W")
		name="ar725w"
		;;
	*"AirLive Air3GII")
		name="air3gii"
		;;
	*"Edimax BR-6425")
		name="br6425"
		;;
	*"Allnet ALL0239-3G")
		name="all0239-3g"
		;;
	*"Allnet ALL0256N")
		name="all0256n"
		;;
	*"Allnet ALL5002")
		name="all5002"
		;;
	*"Allnet ALL5003")
		name="all5003"
		;;
	*"ARC FreeStation5")
		name="freestation5"
		;;
	*"Archer C20i")
		name="c20i"
		;;
	*"Argus ATP-52B")
		name="argus-atp52b"
		;;
	*"AsiaRF AWM002 EVB")
		name="awm002-evb"
		;;
	*"AsiaRF AWM003 EVB")
		name="awm003-evb"
		;;
	*"AsiaRF AWAPN2403")
		name="awapn2403"
		;;
	*"Asus WL-330N")
		name="wl-330n"
		;;
	*"Asus WL-330N3G")
		name="wl-330n3g"
		;;
	*"Alpha ASL26555")
		name="asl26555"
		;;
	*"Aztech HW550-3G")
		name="hw550-3g"
		;;
	*"AXIMCom MR-102N")
		name="mr-102n"
		;;
	*"Buffalo WSR-600DHP")
		name="wsr-600"
		;;
	*"Buffalo WSR-1166DHP")
		name="wsr-1166"
		;;
	*"Comfast CF-WR800N")
		name="cf-wr800n"
		;;
	*"Firefly FireWRT")
		name="firewrt"
		;;
	*"CY-SWR1100")
		name="cy-swr1100"
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
	*"DIR-620 A1")
		name="dir-620-a1"
		;;
	*"DIR-620 D1")
		name="dir-620-d1"
		;;
	*"DIR-615 H1")
		name="dir-615-h1"
		;;
	*"DIR-615 D")
		name="dir-615-d"
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
	*"DAP-1350")
		name="dap-1350"
		;;
	*"ESR-9753")
		name="esr-9753"
		;;
	*"EASYACC WI-STOR WIZARD 8800")
		name="wizard8800"
		;;
	*"Edimax BR-6475nD")
		name="br-6475nd"
		;;
	*"F7C027")
		name="f7c027"
		;;
	*"F5D8235 v1")
		name="f5d8235-v1"
		;;
	*"F5D8235 v2")
		name="f5d8235-v2"
		;;
	*"Hauppauge Broadway")
		name="broadway"
		;;
	*"Huawei D105")
		name="d105"
		;;
	*"La Fonera 2.0N")
		name="fonera20n"
		;;
	*"Asus RT-N14U")
		name="rt-n14u"
		;;
	*"Asus RT-N13U")
		name="rt-n13u"
		;;
	*"MoFi Network MOFI3500-3GN")
		name="mofi3500-3gn"
		;;
	*"HILINK HLK-RM04")
		name="hlk-rm04"
		;;
	*"HooToo HT-TM02")
		name="ht-tm02"
		;;
	*"HAME MPR-A1")
 		name="mpr-a1"
 		;;
	*"HAME MPR-A2")
 		name="mpr-a2"
 		;;
	*"Kingston MLW221")
		name="mlw221"
		;;
	*"Kingston MLWG2")
		name="mlwg2"
		;;
	*"Linksys E1700")
		name="e1700"
		;;
	*"Linksys RE6500")
		name="re6500"
		;;
	*"Planex MZK-750DHP")
		name="mzk-750dhp"
		;;
	*"Microduino MicroWRT")
		name="microwrt"
		;;
	*"NBG-419N")
		name="nbg-419n"
		;;
	*"Netgear WNCE2001")
		name="wnce2001"
		;;
	*"NexAira BC2")
		name="bc2"
		;;
	*"Nexx WT1520")
		name="wt1520"
		;;
	*"Nexx WT3020")
		name="wt3020"
		;;
	*"NW718")
		name="nw718"
		;;
	*"Intenso Memory 2 Move")
		name="m2m"
		;;
	*"Omnima EMB HPM")
		name="omni-emb-hpm"
		;;
	*"Omnima MiniEMBWiFi")
		name="omni-emb"
		;;
	*"Omnima MiniPlug")
		name="omni-plug"
		;;
	*"OLinuXino-RT5350F")
		name="olinuxino-rt5350f"
		;;
	*"OLinuXino-RT5350F-EVB")
		name="olinuxino-rt5350f-evb"
		;;
	*"PBR-M1")
		name="pbr-m1"
		;;
	*"Petatel PSR-680W"*)
		name="psr-680w"
		;;
	*"Planex MZK-W300NH2"*)
		name="mzk-w300nh2"
		;;
	*"Poray IP2202")
		name="ip2202"
		;;
	*"Poray M3")
		name="m3"
		;;
	*"Poray M4")
		name="m4"
		;;
	*"Poray X5")
		name="x5"
		;;
	*"Poray X8")
		name="x8"
		;;
	*"PWH2004")
		name="pwh2004"
		;;
	*"Asus RP-N53")
		name="rp-n53"
		;;
	*"Ralink MT7620a + MT7530 evaluation board")
		name="mt7620a_mt7530"
		;;
	*"RT-G32 B1")
		name="rt-g32-b1"
		;;
	*"RT-N10+")
		name="rt-n10-plus"
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
	*"Skyline SL-R7205"*)
		name="sl-r7205"
		;;
	*"Sparklan WCR-150GN")
		name="wcr-150gn"
		;;
	*"V22RW-2X2")
		name="v22rw-2x2"
		;;
	*"VoCore")
		name="vocore"
		;;
	*"W502U")
		name="w502u"
		;;
	*"WMR-300")
		name="wmr300"
		;;
	*"WHR-300HP2")
		name="whr-300hp2"
		;;
	*"WHR-600D")
		name="whr-600d"
		;;
	*"WHR-1166D")
		name="whr-1166d"
		;;
	*"WHR-G300N")
		name="whr-g300n"
		;;
	*"Sitecom WL-341 v3")
		name="wl341v3"
		;;
	*"Sitecom WL-351 v1 002")
		name="wl-351"
		;;
	*"Tenda 3G300M")
		name="3g300m"
		;;
	*"Tenda 3G150B")
		name="3g150b"
		;;
	*"Tenda W306R V2.0")
		name="w306r-v20"
		;;
	*"Tenda W150M")
		name="w150m"
		;;
	*"TEW-691GR")
		name="tew-691gr"
		;;
	*"TEW-692GR")
		name="tew-692gr"
		;;
	*"Ralink V11ST-FE")
		name="v11st-fe"
		;;
	*"WLI-TX4-AG300N")
		name="wli-tx4-ag300n"
		;;
	*"WZR-AGL300NH")
		name="wzr-agl300nh"
		;;
	*"WR512-3GN-like router")
		name="wr512-3gn"
		;;
	*"UR-326N4G Wireless N router")
		name="ur-326n4g"
		;;
	*"UR-336UN Wireless N router")
		name="ur-336un"
		;;
	*"AWB WR6202")
		name="wr6202"
		;;
	*"XDX RN502J")
		name="xdxrn502j"
		;;
	*"HG255D")
		name="hg255d"
		;;
	*"V22SG")
		name="v22sg"
		;;
	*"WRTNODE")
		name="wrtnode"
		;;
	*"Wansview NCS601W")
		name="ncs601w"
		;;
	*"Xiaomi MiWiFi Mini")
		name="xiaomi-miwifi-mini"
		;;
	*"Sercomm NA930")
		name="na930"
		;;
	*"Zbtlink ZBT-WA05")
		name="zbt-wa05"
		;;
	*"ZBT WR8305RT")
		name="wr8305rt"
		;;
	*"ZTE Q7")
		name="zte-q7"
		;;
	*"Lenovo Y1")
		name="y1"
		;;
	*"Lenovo Y1S")
		name="y1s"
		;;
	*"Mediatek MT7621 evaluation board")
		name="mt7621"
		;;
	*"Mediatek MT7628AN evaluation board")
		name="mt7628"
		;;
	*"MediaTek LinkIt Smart7688")
		linkit="$(dd bs=1 skip=1024 count=12 if=/dev/mtd2 2> /dev/null)"
		if [ "${linkit}" = "LINKITS7688D" ]; then
			name="linkits7688d"
			RAMIPS_MODEL="${machine} DUO"
		else
			name="linkits7688"
		fi
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
