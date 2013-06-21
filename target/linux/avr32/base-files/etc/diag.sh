#!/bin/sh
# Copyright (C) 2013 OpenWrt.org

. /lib/functions/leds.sh

set_state() {
	status_led="sys"

	case "$1" in
	preinit)
		status_led_blink_preinit
		;;

	failsafe)
		status_led_blink_failsafe
		;;

	done)
		status_led_on
		;;
	esac
}
