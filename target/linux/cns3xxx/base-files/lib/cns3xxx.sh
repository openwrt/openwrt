#!/bin/sh
#
# Copyright (C) 2012 OpenWrt.org
#

cns3xxx_board_name() {
	local machine
	local name

	machine=$(awk 'BEGIN{FS="[ \t]+:[ \t]"} /Hardware/ {print $2}' /proc/cpuinfo)

	case "$machine" in
		"Gateworks Corporation Laguna"*)
			name="laguna"
			;;
		*)
			name="generic";
			;;
	esac
	
	echo $name
}
