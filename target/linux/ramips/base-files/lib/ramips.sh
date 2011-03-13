#!/bin/sh
#
# Copyright (C) 2010 OpenWrt.org
#

ramips_board_name() {
	local machine
	local name

	machine=$(awk 'BEGIN{FS="[ \t]+:[ \t]"} /machine/ {print $2}' /proc/cpuinfo)

	case "$machine" in
	*"DIR-300 revB")
		name="dir-300-b1"
		;;
	*"La Fonera 2.0N")
		name="fonera20n"
		;;
	*"V22RW-2X2")
		name="v22rw-2x2"
		;;
	*"WHR-G300N")
		name="whr-g300n"
		;;
	*"Aztech HW550-3G")
		name="hw550-3g"
		;;
	*)
		name="generic"
		;;
	esac

	echo $name
}
