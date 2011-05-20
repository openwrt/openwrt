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
	"bcm63xx/96348GW "*)
		status_led="power"
		brcm63xx_has_reset_button="true"
		ifname=eth1
		;;
	"bcm63xx/GW6000 "*)
		brcm63xx_has_reset_button="true"
		ifname=eth1
		;;
	"bcm63xx/GW6200 "*)
		status_led="line1"
		status_led2="tel"
		brcm63xx_has_reset_button="true"
		ifname=eth1
		;;
	"bcm63xx/96348GW-11 "*)
		status_led="power"
		brcm63xx_has_reset_button="true"
		ifname=eth1
		;;
	"bcm63xx/CT536_CT5621 "*)
		status_led="power"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	"bcm63xx/SPW500V "*)
		status_led="power:green"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	"bcm63xx/AW4139 "*)
		status_led="dsl-274xb:green:power"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	"bcm63xx/96358-502V "*)
		status_led="spw303v:green:power+adsl"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	"bcm63xx/CPVA642 "* )
		status_led="power:green"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	"bcm63xx/D-4P-W "*)
		status_led="power:green"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	*)
		;;
	esac
}

brcm63xx_detect
