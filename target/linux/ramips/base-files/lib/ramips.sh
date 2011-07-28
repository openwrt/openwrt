#!/bin/sh
#
# Copyright (C) 2010 OpenWrt.org
#

ramips_board_name() {
	local machine
	local name

	machine=$(awk 'BEGIN{FS="[ \t]+:[ \t]"} /machine/ {print $2}' /proc/cpuinfo)

	case "$machine" in
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
	*"F5D8235 v2")
		name="f5d8235-v2"
		;;
	*"La Fonera 2.0N")
		name="fonera20n"
		;;
	*"MoFi Network MOFI3500-3GN")
		name="mofi3500-3gn"
		;;
	*"NW718")
		name="nw718"
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
	*"WHR-G300N")
		name="whr-g300n"
		;;
	*"WZR-AGL300NH")
		name="wzr-agl300nh"
		;;
	*)
		name="generic"
		;;
	esac

	echo $name
}
