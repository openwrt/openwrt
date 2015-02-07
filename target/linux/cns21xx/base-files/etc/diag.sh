#!/bin/sh
# Copyright (C) 2010-2013 OpenWrt.org

. /lib/functions/leds.sh
. /lib/cns21xx.sh

get_status_led() {
	case $(get_board_name) in
	ns-k330)
		status_led="ns-k330:red:link"
		;;
	esac;
}

set_state() {
	get_status_led

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
