#!/bin/sh
#
# Copyright (C) 2015 OpenWrt.org
#

brcm2708_board_detect() {
	local machine
	local name

	machine=$(awk 'BEGIN{FS="[ \t]+:[ \t]"} /Hardware/ {print $2}' /proc/cpuinfo)

	case "$machine" in
	BCM2708)
		name="Raspberry Pi"
		;;
	BCM2709)
		name="Raspberry Pi 2"
		;;
	esac

	[ -z "$name" ] && name="unknown"

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	echo "$machine" > /tmp/sysinfo/board_name
	echo "$name" > /tmp/sysinfo/model
}

brcm2708_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)
	[ -z "$name" ] && name="unknown"

	echo "$name"
}
