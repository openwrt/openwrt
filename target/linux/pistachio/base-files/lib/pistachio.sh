#!/bin/sh
#
# Copyright (C) 2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

PISTACHIO_BOARD_NAME=
PISTACHIO_MODEL=

pistachio_board_detect() {
	local machine
	local name

	machine=$(cat /proc/device-tree/model)

	case "$machine" in
	"IMG Marduk (Creator Ci40)")
		name="marduk"
		;;
	*)
		name="generic"
		;;
	esac

	[ -z "$PISTACHIO_BOARD_NAME" ] && PISTACHIO_BOARD_NAME="$name"
	[ -z "$PISTACHIO_MODEL" ] && PISTACHIO_MODEL="$machine"

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	echo "$PISTACHIO_BOARD_NAME" > /tmp/sysinfo/board_name
	echo "$PISTACHIO_MODEL" > /tmp/sysinfo/model
}
