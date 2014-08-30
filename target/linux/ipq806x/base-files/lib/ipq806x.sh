#!/bin/sh
#
# Copyright (c) 2014 The Linux Foundation. All rights reserved.
# Copyright (C) 2011 OpenWrt.org
#

IPQ806X_BOARD_NAME=
IPQ806X_MODEL=

ipq806x_board_detect() {
	local machine
	local name

	machine=$(cat /proc/device-tree/model)

	case "$machine" in
	*"AP148")
		name="ap148"
		;;
	esac

	[ -z "$name" ] && name="unknown"

	[ -z "$IPQ806X_BOARD_NAME" ] && IPQ806X_BOARD_NAME="$name"
	[ -z "$IPQ806X_MODEL" ] && IPQ806X_MODEL="$machine"

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	echo "$IPQ806X_BOARD_NAME" > /tmp/sysinfo/board_name
	echo "$IPQ806X_MODEL" > /tmp/sysinfo/model
}

ipq806x_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)
	[ -z "$name" ] && name="unknown"

	echo "$name"
}
