#!/bin/sh
#
# Copyright (C) 2010-2013 OpenWrt.org
#

SOCFPGA_BOARD_NAME=
SOCFPGA_MODEL=

socfpga_board_detect() {
	local machine
	local name

	machine=$(cat /proc/device-tree/model)

	case "$machine" in
	"Terasic SoCkit")
		name="socfpga-sockit"
		;;
	*)
		name="generic"
		;;
	esac

	[ -z "$SOCFPGA_BOARD_NAME" ] && SOCFPGA_BOARD_NAME="$name"
	[ -z "$SOCFPGA_MODEL" ] && SOCFPGA_MODEL="$machine"

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	echo "$SOCFPGA_BOARD_NAME" > /tmp/sysinfo/board_name
	echo "$SOCFPGA_MODEL" > /tmp/sysinfo/model
}

socfpga_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] || socfpga_board_detect
	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)

	# Name is too generic, use model
	if [ "$name" = "altr,socfpga-cyclone5" ] ; then
		socfpga_board_detect
		name=$(cat /tmp/sysinfo/board_name)
	fi

	[ -z "$name" ] && name="unknown"

	echo "$name"
}
