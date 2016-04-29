#!/bin/sh
#
# Copyright (C) 2016 OpenWrt.org
#

mediatek_board_detect() {
	local machine
	local name

	machine=$(cat /proc/device-tree/model)

	case "$machine" in
	"MediaTek MT7623 evaluation board")
		name="mt7623_evb"
		;;
	"MediaTek MT7623 eMMC evaluation board")
		name="eMMC"
		;;
	"MediaTek MT7623 NAND evaluation board")
		name="NAND"
		;;
	esac

	[ -z "$name" ] && name="unknown"

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	echo "$name" > /tmp/sysinfo/board_name
	echo "$machine" > /tmp/sysinfo/model
}

mediatek_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)
	[ -z "$name" ] && name="unknown"

	echo "$name"
}
