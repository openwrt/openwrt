#!/bin/sh
#
# Copyright (C) 2012 OpenWrt.org
#

ixp4xx_board_name() {
        local machine
        local name

        machine=$(awk 'BEGIN{FS="[ \t]+:[ \t]"} /Hardware/ {print $2}' /proc/cpuinfo)

        case "$machine" in
		"Gateworks Cambria"*)
			name="cambria"
			;;
		"Gateworks Avila"*)
			name="avila"
			;;
		*)
			name="generic";
			;;
	esac

	echo $name
}
