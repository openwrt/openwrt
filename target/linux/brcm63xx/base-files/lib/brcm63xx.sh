#!/bin/sh
#
# Copyright (C) 2007 OpenWrt.org
#
#

board_name=""
status_led=""
status_led2=""
sys_mtd_part=""
brcm63xx_has_reset_button=""

brcm63xx_detect() {
	board_name=$(awk 'BEGIN{FS="[ \t]+:[ \t]"} /system type/ {print $2}' /proc/cpuinfo)

	case "$board_name" in
	"bcm63xx/96348GW"*)
		status_led="tel"
		status_led2="line1"
		brcm63xx_has_reset_button="true"
		ifname=eth1
		;;
	*)
		;;
	esac
}

brcm63xx_detect
