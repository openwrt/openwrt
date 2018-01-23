#!/bin/sh
#
# Copyright (C) 2018 OpenWrt.org
#

octeontx_board_detect() {
	local machine
	local name

	machine=$(cat /proc/device-tree/model)
	board=$(cat /proc/device-tree/board)

	case "$machine" in
	"Cavium ThunderX CN81XX board")
		name="cn81xx"
		;;

	*)
		name="generic"
		;;
	esac

	case "$board" in
	GW630*)
		name="gw630x"
		;;
	esac

	[ -e "/tmp/sysinfo" ] || mkdir -p "/tmp/sysinfo"

	echo "$name" > /tmp/sysinfo/board_name
	echo "$machine" > /tmp/sysinfo/model
}

octeontx_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] || octeontx_board_detect
	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)
	[ -z "$name" ] && name="unknown"

	echo "$name"
}
