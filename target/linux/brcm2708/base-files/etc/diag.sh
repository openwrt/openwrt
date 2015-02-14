#!/bin/sh
#
# Copyright (C) 2015 OpenWrt.org
#

. /lib/functions/leds.sh

set_state() {
	status_led="led0"

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
