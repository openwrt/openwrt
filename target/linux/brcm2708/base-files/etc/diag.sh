#!/bin/sh
# Copyright (C) 2015-2016 OpenWrt.org

. /lib/functions/leds.sh
. /lib/brcm2708.sh

set_state() {
	case "$(brcm2708_board_name)" in
	rpi-b |\
	rpi-cm)
		status_led="led0"
		;;
	rpi-b-plus |\
	rpi-2-b |\
	rpi-3-b)
		status_led="led1"
		;;
	esac

	case "$1" in
	preinit)
		status_led_blink_preinit
		;;
	failsafe)
		status_led_blink_failsafe
		;;
	preinit_regular)
		status_led_blink_preinit_regular
		;;
	done)
		status_led_on
		;;
	esac
}
