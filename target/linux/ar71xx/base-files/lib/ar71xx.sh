#!/bin/sh
#
# Copyright (C) 2009-2011 OpenWrt.org
#

AR71XX_BOARD_NAME=
AR71XX_MODEL=

ar71xx_get_mtd_offset_size_format() {
	local mtd="$1"
	local offset="$2"
	local size="$3"
	local format="$4"
	local dev

	dev=$(find_mtd_part $mtd)
	[ -z "$dev" ] && return

	dd if=$dev bs=1 skip=$offset count=$size 2>/dev/null | hexdump -v -e "1/1 \"$format\""
}

ar71xx_get_mtd_part_magic() {
	local mtd="$1"
	ar71xx_get_mtd_offset_size_format "$mtd" 0 4 %02x
}

wndr3700_board_detect() {
	local machine="$1"
	local magic
	local name

	name="wndr3700"

	magic="$(ar71xx_get_mtd_part_magic firmware)"
	case $magic in
	"33373030")
		machine="NETGEAR WNDR3700"
		;;
	"33373031")
		local model
		model=$(ar71xx_get_mtd_offset_size_format art 56 10 %c)
		if [ -z "$model" ] || [ "$model" = $'\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' ]; then
			machine="NETGEAR WNDR3700v2"
		elif [ -z "$model" ] || [ "$model" = $'\xff\xff\xff\xff\xff\xff\xff\xff\xffN' ]; then
			machine="NETGEAR WNDRMAC"
		else
			machine="NETGEAR $model"
		fi
		;;
	esac

	AR71XX_BOARD_NAME="$name"
	AR71XX_MODEL="$machine"
}

cybertan_get_hw_magic() {
	local part

	part=$(find_mtd_part firmware)
	[ -z "$part" ] && return 1

	dd bs=8 count=1 skip=0 if=$part 2>/dev/null | hexdump -v -n 8 -e '1/1 "%02x"'
}

tplink_get_hwid() {
	local part

	part=$(find_mtd_part firmware)
	[ -z "$part" ] && return 1

	dd if=$part bs=4 count=1 skip=16 2>/dev/null | hexdump -v -n 4 -e '1/1 "%02x"'
}

tplink_get_mid() {
	local part

	part=$(find_mtd_part firmware)
	[ -z "$part" ] && return 1

	dd if=$part bs=4 count=1 skip=17 2>/dev/null | hexdump -v -n 4 -e '1/1 "%02x"'
}

tplink_board_detect() {
	local model="$1"
	local hwid
	local hwver

	hwid=$(tplink_get_hwid)
	mid=$(tplink_get_mid)
	hwver=${hwid:6:2}
	hwver="v${hwver#0}"

	case "$hwid" in
	"3C0001"*)
		model="OOLITE"
		;;
	"070300"*)
		model="TP-Link TL-WR703N"
		;;
	"071000"*)
		model="TP-Link TL-WR710N"
		;;
	"072001"*)
		model="TP-Link TL-WR720N"
		;;
	"070100"*)
		model="TP-Link TL-WA701N/ND"
		;;
	"073000"*)
		model="TP-Link TL-WA730RE"
		;;
	"074000"*)
		model="TP-Link TL-WR740N/ND"
		;;
	"074100"*)
		model="TP-Link TL-WR741N/ND"
		;;
	"074300"*)
		model="TP-Link TL-WR743N/ND"
		;;
	"075000"*)
		model="TP-Link TL-WA750RE"
		;;
	"751000"*)
		model="TP-Link TL-WA7510N"
		;;
	"080100"*)
		model="TP-Link TL-WA801N/ND"
		;;
	"083000"*)
		model="TP-Link TL-WA830RE"
		;;
	"084100"*)
		model="TP-Link TL-WR841N/ND"
		;;
	"084200"*)
		model="TP-Link TL-WR842N/ND"
		;;
	"085000"*)
		model="TP-Link TL-WA850RE"
		;;
	"090100"*)
		model="TP-Link TL-WA901N/ND"
		;;
	"094100"*)
		if [ "$hwid" == "09410002" -a "$mid" == "00420001" ]; then
			model="Rosewill RNX-N360RT"
			hwver=""
		else
			model="TP-Link TL-WR941N/ND"
		fi
		;;
	"104100"*)
		model="TP-Link TL-WR1041N/ND"
		;;
	"104300"*)
		model="TP-Link TL-WR1043N/ND"
		;;
	"254300"*)
		model="TP-Link TL-WR2543N/ND"
		;;
	"001001"*)
		model="TP-Link TL-MR10U"
		;;
	"001101"*)
		model="TP-Link TL-MR11U"
		;;
	"001301"*)
		model="TP-Link TL-MR13U"
		;;
	"302000"*)
		model="TP-Link TL-MR3020"
		;;
	"304000"*)
		model="TP-Link TL-MR3040"
		;;
	"322000"*)
		model="TP-Link TL-MR3220"
		;;
	"342000"*)
		model="TP-Link TL-MR3420"
		;;
	"350000"*)
		model="TP-Link TL-WDR3500"
		;;
	"360000"*)
		model="TP-Link TL-WDR3600"
		;;
	"430000"*)
		model="TP-Link TL-WDR4300"
		;;
	"430080"*)
		iw reg set IL
		model="TP-Link TL-WDR4300 (IL)"
		;;
	"431000"*)
		model="TP-Link TL-WDR4310"
		;;
	"49000002")
		model="TP-Link TL-WDR4900"
		;;
	"453000"*)
		model="MERCURY MW4530R"
		;;
	*)
		hwver=""
		;;
	esac

	AR71XX_MODEL="$model $hwver"
}

ar71xx_board_detect() {
	local machine
	local name

	machine=$(awk 'BEGIN{FS="[ \t]+:[ \t]"} /machine/ {print $2}' /proc/cpuinfo)

	case "$machine" in
	*"Oolite V1.0")
		name="oolite"
		;;
	*"AirRouter")
		name="airrouter"
		;;
	*"ALFA Network AP96")
		name="alfa-ap96"
		;;
	*"ALFA Network N2/N5")
		name="alfa-nx"
		;;
	*ALL0258N)
		name="all0258n"
		;;
	*ALL0305)
		name="all0305"
		;;
	*ALL0315N)
		name="all0315n"
		;;
	*AP113)
		name="ap113"
		;;
	*AP121)
		name="ap121"
		;;
	*AP121-MINI)
		name="ap121-mini"
		;;
	*"AP132 reference board")
		name="ap132"
		;;
	*"AP136-010 reference board")
		name="ap136-010"
		;;
	*"AP136-020 reference board")
		name="ap136-020"
		;;
	*"AP135-020 reference board")
		name="ap135-020"
		;;
	*AP81)
		name="ap81"
		;;
	*AP83)
		name="ap83"
		;;
	*"Archer C7")
		name="archer-c7"
		;;
	*"Atheros AP96")
		name="ap96"
		;;
	*AW-NR580)
		name="aw-nr580"
		;;
	*CAP4200AG)
		name="cap4200ag"
		;;
	*"DB120 reference board")
		name="db120"
		;;
	*"DIR-505 rev. A1")
		name="dir-505-a1"
		;;
	*"DIR-600 rev. A1")
		name="dir-600-a1"
		;;
	*"DIR-615 rev. E1")
		name="dir-615-e1"
		;;
	*"DIR-615 rev. E4")
		name="dir-615-e4"
		;;
	*"DIR-825 rev. B1")
		name="dir-825-b1"
		;;
	*"DIR-825 rev. C1")
		name="dir-825-c1"
		;;
	*"DIR-835 rev. A1")
		name="dir-835-a1"
		;;
	*"Dragino v2")
		name="dragino2"
		;;
	*EAP7660D)
		name="eap7660d"
		;;
	*JA76PF)
		name="ja76pf"
		;;
	*JA76PF2)
		name="ja76pf2"
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
	*"Hornet-UB")
		local size
		size=$(awk '/firmware/ { print $2 }' /proc/mtd)

		if [ "x$size" = "x00790000" ]; then
			name="hornet-ub"
		fi

		if [ "x$size" = "x00f90000" ]; then
			name="hornet-ub-x2"
		fi
		;;
	*LS-SR71)
		name="ls-sr71"
		;;
	*MR600v2)
		name="mr600v2"
		;;
	*MR600)
		name="mr600"
		;;
	*"My Net N600")
		name="mynet-n600"
		;;
	*"My Net N750")
		name="mynet-n750"
		;;
	*"WD My Net Wi-Fi Range Extender")
		name="mynet-rext"
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
	*"Zyxel NBG6716")
		name="nbg6716"
		;;
	*OM2P)
		name="om2p"
		;;
	*OM2Pv2)
		name="om2pv2"
		;;
	*"OM2P HS")
		name="om2p-hs"
		;;
	*"OM2P HSv2")
		name="om2p-hsv2"
		;;
	*"OM2P LC")
		name="om2p-lc"
		;;
	*PB42)
		name="pb42"
		;;
	*"PB44 reference board")
		name="pb44"
		;;
	*PB92)
		name="pb92"
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
	*"RouterBOARD 435G")
		name="rb-435g"
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
	*"RouterBOARD 493G")
		name="rb-493g"
		;;
	*"RouterBOARD 750")
		name="rb-750"
		;;
	*"RouterBOARD 750GL")
		name="rb-750gl"
		;;
	*"RouterBOARD 751")
		name="rb-751"
		;;
	*"RouterBOARD 751G")
		name="rb-751g"
		;;
	*"RouterBOARD 911G-2HPnD")
		name="rb-911g-2hpnd"
		;;
	*"RouterBOARD 911G-5HPnD")
		name="rb-911g-5hpnd"
		;;
	*"RouterBOARD 912UAG-2HPnD")
		name="rb-912uag-2hpnd"
		;;
	*"RouterBOARD 912UAG-5HPnD")
		name="rb-912uag-5hpnd"
		;;
	*"RouterBOARD 951G-2HnD")
		name="rb-951g-2hnd"
		;;
	*"RouterBOARD 951Ui-2HnD")
		name="rb-951ui-2hnd"
		;;
	*"RouterBOARD 2011L")
		name="rb-2011l"
		;;
	*"RouterBOARD 2011UAS")
		name="rb-2011uas"
		;;
	*"RouterBOARD 2011UAS-2HnD")
		name="rb-2011uas-2hnd"
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
	*RW2458N)
		name="rw2458n"
		;;
	*TEW-632BRP)
		name="tew-632brp"
		;;
	*TEW-673GRU)
		name="tew-673gru"
		;;
	*TEW-712BR)
		name="tew-712br"
		;;
	*TEW-732BR)
		name="tew-732br"
		;;
	*"TL-WR1041N v2")
		name="tl-wr1041n-v2"
		;;
	*TL-WR1043ND)
		name="tl-wr1043nd"
		;;
	*"TL-WR1043ND v2")
		name="tl-wr1043nd-v2"
		;;
	*TL-WR2543N*)
		name="tl-wr2543n"
		;;
	*"DIR-615 rev. C1")
		name="dir-615-c1"
		;;
	*TL-MR3020)
		name="tl-mr3020"
		;;
	*TL-MR3040)
		name="tl-mr3040"
		;;
	*"TL-MR3040 v2")
		name="tl-mr3040-v2"
		;;
	*TL-MR3220)
		name="tl-mr3220"
		;;
	*"TL-MR3220 v2")
		name="tl-mr3220-v2"
		;;
	*TL-MR3420)
		name="tl-mr3420"
		;;
	*"TL-MR3420 v2")
		name="tl-mr3420-v2"
		;;
	*TL-WA750RE)
		name="tl-wa750re"
		;;
	*"TL-WA7510N v1")
		name="tl-wa7510n"
		;;
	*TL-WA850RE)
		name="tl-wa850re"
		;;
	*"TL-WA801ND v2")
		name="tl-wa801nd-v2"
		;;
	*TL-WA901ND)
		name="tl-wa901nd"
		;;
	*"TL-WA901ND v2")
		name="tl-wa901nd-v2"
		;;
	*"TL-WA901ND v3")
		name="tl-wa901nd-v3"
		;;
	*"TL-WDR3500")
		name="tl-wdr3500"
		;;
	*"TL-WDR3600/4300/4310")
		name="tl-wdr4300"
		;;
	*"TL-WDR4900 v2")
		name="tl-wdr4900-v2"
		;;
	*TL-WR741ND)
		name="tl-wr741nd"
		;;
	*"TL-WR741ND v4")
		name="tl-wr741nd-v4"
		;;
	*"TL-WR841N v1")
		name="tl-wr841n-v1"
		;;
	*"TL-WR841N/ND v7")
		name="tl-wr841n-v7"
		;;
	*"TL-WR841N/ND v8")
		name="tl-wr841n-v8"
		;;
	*"TL-WR841N/ND v9")
		name="tl-wr841n-v9"
		;;
	*"TL-WR842N/ND v2")
		name="tl-wr842n-v2"
		;;
	*TL-WR941ND)
		name="tl-wr941nd"
		;;
	*"TL-WR703N v1")
		name="tl-wr703n"
		;;
	*"TL-WR710N v1")
		name="tl-wr710n"
		;;
	*"TL-WR720N v3")
		name="tl-wr720n-v3"
		;;
	*"TL-MR10U")
		name="tl-mr10u"
		;;
	*"TL-MR11U")
		name="tl-mr11u"
		;;
	*"TL-MR13U")
		name="tl-mr13u"
		;;
	*UniFi)
		name="unifi"
		;;
	*"UniFi AP Pro")
		name="uap-pro"
		;;
	*WHR-G301N)
		name="whr-g301n"
		;;
	*WHR-HP-GN)
		name="whr-hp-gn"
		;;
	*WLAE-AG300N)
		name="wlae-ag300n"
		;;
	*"UniFiAP Outdoor")
		name="unifi-outdoor"
		;;
	*WP543)
		name="wp543"
		;;
	*WPE72)
		name="wpe72"
		;;
	*WNDAP360)
		name="wndap360"
		;;
	*"WNDR3700/WNDR3800/WNDRMAC")
		wndr3700_board_detect "$machine"
		;;
	*"WNDR4300")
		name="wndr4300"
		;;
	*"WNR2000 V3")
		name="wnr2000-v3"
		;;
	*WNR2000)
		name="wnr2000"
		;;
	*WNR2200)
		name="wnr2200"
		;;
	*"WNR612 V2")
		name="wnr612-v2"
		;;
	*WRT160NL)
		name="wrt160nl"
		;;
	*WRT400N)
		name="wrt400n"
		;;
	*"WZR-HP-AG300H/WZR-600DHP")
		name="wzr-hp-ag300h"
		;;
	*WZR-HP-G300NH)
		name="wzr-hp-g300nh"
		;;
	*WZR-HP-G450H)
		name="wzr-hp-g450h"
		;;
	*WZR-HP-G300NH2)
		name="wzr-hp-g300nh2"
		;;
	*WHR-HP-G300N)
		name="whr-hp-g300n"
		;;
	*ZCN-1523H-2)
		name="zcn-1523h-2"
		;;
	*ZCN-1523H-5)
		name="zcn-1523h-5"
		;;
	*EmbWir-Dorin)
		name="ew-dorin"
		;;
	*EmbWir-Dorin-Router)
		name="ew-dorin-router"
		;;
	"8devices Carambola2"*)
		name="carambola2"
		;;
	*"Sitecom WLR-8100")
		name="wlr8100"
		;;
	*"BHU BXU2000n-2 rev. A1")
		name="bxu2000n-2-a1"
		;;
	*"HiWiFi HC6361")
		name="hiwifi-hc6361"
		;;
	esac

	case "$machine" in
	*TL-WR* | *TL-WA* | *TL-MR* | *TL-WD*)
		tplink_board_detect "$machine"
		;;
	esac

	[ -z "$name" ] && name="unknown"

	[ -z "$AR71XX_BOARD_NAME" ] && AR71XX_BOARD_NAME="$name"
	[ -z "$AR71XX_MODEL" ] && AR71XX_MODEL="$machine"

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	echo "$AR71XX_BOARD_NAME" > /tmp/sysinfo/board_name
	echo "$AR71XX_MODEL" > /tmp/sysinfo/model
}

ar71xx_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)
	[ -z "$name" ] && name="unknown"

	echo "$name"
}
