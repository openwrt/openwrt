#!/bin/sh
#
# Copyright (C) 2010 OpenWrt.org
#

ramips_get_mac_binary()
{
	local mtdname="$1"
	local seek="$2"
	local part

	. /lib/functions.sh

	part=$(find_mtd_part "$mtdname")
	if [ -z "$part" ]; then
		echo "ramips_get_mac_binary: partition $mtdname not found!" >&2
		return
	fi

	dd bs=1 skip=$seek count=6 if=$part 2>/dev/null | /usr/sbin/maccalc bin2mac
}

ramips_get_mac_nvram()
{
	local mtdname="$1"
	local key="$2"
	local part
	local mac_dirty

	. /lib/functions.sh

	part=$(find_mtd_part "$mtdname")
	if [ -z "$part" ]; then
		echo "ramips_get_mac_nvram: partition $mtdname not found!" >&2
		return
	fi

	mac_dirty=$(strings "$part" | sed -n 's/'"$key"'=//p')
	# "canonicalize" mac
	/usr/sbin/maccalc add "$mac_dirty" 0
}

ramips_board_name() {
	local machine
	local name

	machine=$(awk 'BEGIN{FS="[ \t]+:[ \t]"} /machine/ {print $2}' /proc/cpuinfo)

	case "$machine" in
	*"8devices Carambola")
		name="carambola"
		;;
	*"Edimax 3g-6200n")
		name="3g-6200n"
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
	*"ARC FreeStation5")
		name="freestation5"
		;;
	*"Argus ATP-52B")
		name="argus-atp52b"
		;;
	*"BR6524N")
		name="br6524n"
		;;
	*"Asus WL-330N")
		name="wl-330n"
		;;
	*"Asus WL-330N3G")
		name="wl-330n3g"
		;;
	*"Aztech HW550-3G")
		name="hw550-3g"
		;;
	*"DIR-300 B1")
		name="dir-300-b1"
		;;
	*"DIR-600 B1")
		name="dir-600-b1"
		;;
	*"DIR-600 B2")
		name="dir-600-b2"
		;;
	*"DIR-620 A1")
		name="dir-620-a1"
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
        *"DAP-1350")
                name="dap-1350"
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
	*"Hauppauge Broadway")
		name="broadway"
		;;
	*"La Fonera 2.0N")
		name="fonera20n"
		;;
	*"Asus RT-N13U")
		name="rt-n13u"
		;;
	*"MoFi Network MOFI3500-3GN")
		name="mofi3500-3gn"
		;;
	*"NBG-419N")
		name="nbg-419n"
		;;
	*"NexAira BC2")
		name="bc2"
		;;
	*"NW718")
		name="nw718"
		;;
	*"Omnima MiniEMBWiFi")
		name="omni-emb"
		;;
	*"Petatel PSR-680W"*)
		name="psr-680w"
		;;
	*"Planex MZK-W300NH2"*)
		name="mzk-w300nh2"
		;;
	*"PWH2004")
		name="pwh2004"
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
	*"Skyline SL-R7205"*)
		name="sl-r7205"
		;;
	*"Sparklan WCR-150GN")
		name="wcr-150gn"
		;;
	*"V22RW-2X2")
		name="v22rw-2x2"
		;;
	*"W502U")
		name="w502u"
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
	*"Tenda W306R V2.0")
		name="w306r-v20"
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
	*"UR-336UN Wireless N router")
		name="ur-336un"
		;;
	*"AWB WR6202")
		name="wr6202"
		;;
	*"XDX RN502J")
		name="xdxrn502j"
		;;
	*)
		name="generic"
		;;
	esac

	echo $name
}
