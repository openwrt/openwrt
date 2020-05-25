#!/bin/sh
# Copyright (C) 2013-2014 OpenWrt.org

. /lib/functions.sh
. /lib/functions/leds.sh

get_status_led_platform() {
	case $(board_name) in
	i2se,duckbill*)
		status_led="duckbill:green:status"
		;;
	olimex,imx23-olinuxino)
		status_led="green"
		;;
	*)
		status_led=$(cd /sys/class/leds && ls -1d *:status 2> /dev/null | head -n 1)
		;;
	esac

	boot="$status_led"
	failsafe="$status_led"
	running="$status_led"
	upgrade="$status_led"
}
