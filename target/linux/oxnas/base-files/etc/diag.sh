#!/bin/sh
# Copyright (C) 2009-2013 OpenWrt.org

. /lib/functions/leds.sh
. /lib/oxnas.sh

get_status_led() {
	case $(oxnas_board_name) in
	stg212)
		status_led="zyxel:blue:status"
		;;
	kd20)
		status_led="kd20:blue:status"
		;;
	pogoplugpro | pogoplugv3)
		status_led="pogoplug:blue:internal"
		;;
	esac
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
	done)
		status_led_on
		;;
	esac
}
