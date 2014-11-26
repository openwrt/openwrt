#!/bin/sh
#
# Copyright (C) 2013 OpenWrt.org
#

OXNAS_BOARD_NAME=
OXNAS_MODEL=

oxnas_board_detect() {
	local machine
	local name

	machine=$(cat /proc/device-tree/model)

	case "$machine" in
	*"MitraStar Technology Corp. STG-212"*)
		name="stg212"
		;;
	*"Shuttle KD20"*)
		name="kd20"
		;;
	*"Pogoplug Pro"*)
		name="pogoplugpro"
		;;
	*"Pogoplug V3"*)
		name="pogoplugv3"
		;;
	esac

	[ -z "$name" ] && name="unknown"

	[ -z "$OXNAS_BOARD_NAME" ] && OXNAS_BOARD_NAME="$name"
	[ -z "$OXNAS_MODEL" ] && OXNAS_MODEL="$machine"

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	echo "$OXNAS_BOARD_NAME" > /tmp/sysinfo/board_name
	echo "$OXNAS_MODEL" > /tmp/sysinfo/model
}

oxnas_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)
	[ -z "$name" ] && name="unknown"

	echo "$name"
}
