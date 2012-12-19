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

tplink_get_hwid() {
	local part

	part=$(find_mtd_part firmware)
	[ -z "$part" ] && return 1

	dd if=$part bs=4 count=1 skip=16 2>/dev/null | hexdump -v -n 4 -e '1/1 "%02x"'
}

tplink_board_detect() {
	local model="$1"
	local hwid
	local hwver

	hwid=$(tplink_get_hwid)
	hwver=${hwid:6:2}
	hwver="v${hwver#0}"

	case "$hwid" in
	"070300"*)
		model="TP-Link TL-WR703N"
		;;
	"070100"*)
		model="TP-Link TL-WA701N/ND"
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
	"084100"*)
		model="TP-Link TL-WR841N/ND"
		;;
	"084200"*)
		model="TP-Link TL-WR842N/ND"
		;;
	"090100"*)
		model="TP-Link TL-WA901N/ND"
		;;
	"094100"*)
		model="TP-Link TL-WR941N/ND"
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
	"110101"*)
		model="TP-Link TL-MR11U"
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
	"360000"*)
		model="TP-Link TL-WDR3600"
		;;
	"430000"*)
		model="TP-Link TL-WDR4300"
		;;
	"431000"*)
		model="TP-Link TL-WDR4310"
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
	*"AP136 reference board")
		name="ap136"
		;;
	*AP81)
		name="ap81"
		;;
	*AP83)
		name="ap83"
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
	*"DIR-600 rev. A1")
		name="dir-600-a1"
		;;
	*"DIR-615 rev. E4")
		name="dir-615-e4"
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
		name="hornet-ub"
		;;
	*LS-SR71)
		name="ls-sr71"
		;;
	*MR600)
		name="mr600"
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
	*OM2P)
		name="om2p"
		;;
	*"OM2P HS")
		name="om2p-hs"
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
	*"RouterBOARD 2011L")
		name="rb-2011l"
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
	*"TL-WR1041N v2")
		name="tl-wr1041n-v2"
		;;
	*TL-WR1043ND)
		name="tl-wr1043nd"
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
	*TL-MR3220)
		name="tl-mr3220"
		;;
	*"TL-MR3220 v2")
		name="tl-mr3220-v2"
		;;
	*TL-MR3420)
		name="tl-mr3420"
		;;
	*TL-WA7510N)
		name="tl-wa7510n"
		;;
	*TL-WA901ND)
		name="tl-wa901nd"
		;;
	*"TL-WA901ND v2")
		name="tl-wa901nd-v2"
		;;
	*"TL-WDR3600/4300/4310")
		name="tl-wdr4300"
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
	*TL-WR941ND)
		name="tl-wr941nd"
		;;
	*"TL-WR703N v1")
		name="tl-wr703n"
		;;
	*"TL-MR11U")
		name="tl-mr11u"
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
	*"WNDR3700/WNDR3800/WNDRMAC")
		wndr3700_board_detect "$machine"
		;;
	*"WNDR4300")
		name="wndr4300"
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
	*WZR-HP-AG300H)
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
