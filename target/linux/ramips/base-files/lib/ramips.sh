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
	*"Argus ATP-52B")
		name="argus-atp52b"
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
	*"ESR-9753")
		name="esr-9753"
		;;
	*"F5D8235 v2")
		name="f5d8235-v2"
		;;
	*"La Fonera 2.0N")
		name="fonera20n"
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
	*"PWH2004")
		name="pwh2004"
		;;
	*"RT-G32 B1")
		name="rt-g32-b1"
		;;
	*"RT-N15")
		name="rt-n15"
		;;
	*"WCR-150GN")
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
	*"Sitecom WL-351 v1 002")
		name="wl-351"
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
	*)
		name="generic"
		;;
	esac

	echo $name
}
