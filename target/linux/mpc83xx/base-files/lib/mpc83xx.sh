#!/bin/sh
#
# Copyright (C) 2012 OpenWrt.org
#

MPC83XX_BOARD_NAME=
MPC83XX_MODEL=

mpc83xx_board_detect() {
	local model
	local name

	model=$(awk 'BEGIN{FS="[ \t]+:[ \t]"} /model/ {print $2}' /proc/cpuinfo)

	case "$model" in
	"RB333")
		name="rb333"
		model="MikroTik RouterBOARD 333"
		;;
	"RB600")
		name="rb600"
		model="MikroTik RouterBOARD 600"
		;;
	esac

	[ -z "$name" ] && name="unknown"

	[ -z "$MPC83XX_BOARD_NAME" ] && MPC83XX_BOARD_NAME="$name"
	[ -z "$MPC83XX_MODEL" ] && MPC83XX_MODEL="$model"

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	echo "$MPC83XX_BOARD_NAME" > /tmp/sysinfo/board_name
	echo "$MPC83XX_MODEL" > /tmp/sysinfo/model
}

mpc83xx_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)
	[ -z "$name" ] && name="unknown"

	echo "$name"
}
