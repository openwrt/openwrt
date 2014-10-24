#!/bin/sh
#
# Copyright (C) 2014 OpenWrt.org
#

NETLOGIC_BOARD_NAME=
NETLOGIC_MODEL=

netlogic_board_detect() {
	local machine
	local name

	machine=$(awk 'BEGIN{FS="[ \t:/]+"} /machine/ {print $2}' /proc/cpuinfo)

	case "$machine" in
	*"netlogic,XLP-EVP")
		name="xlp-evp"
		;;
	*"netlogic,XLP-FVP")
		name="xlp-fvp"
		;;
	*"netlogic,XLP-GVP")
		name="xlp-gvp"
		;;
	*"netlogic,XLP-SVP")
		name="xlp-svp"
		;;
	esac

	[ -z "$name" ] && name="unknown"

	[ -z "$NETLOGIC_BOARD_NAME" ] && NETLOGIC_BOARD_NAME="$name"
	[ -z "$NETLOGIC_MODEL" ] && NETLOGIC_MODEL="$machine"

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	echo "$NETLOGIC_BOARD_NAME" > /tmp/sysinfo/board_name
	echo "$NETLOGIC_MODEL" > /tmp/sysinfo/model
}

netlogic_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)
	[ -z "$name" ] && name="unknown"

	echo "$name"
}
