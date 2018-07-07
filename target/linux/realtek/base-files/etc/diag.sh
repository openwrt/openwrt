#!/bin/sh
# Copyright (C) 2009-2013 OpenWrt.org

. /lib/functions/leds.sh
. /lib/realtek.sh

get_status_led() {
	local board=$(realtek_board_name)

	case $board in
	au-home-spot-cube)
		status_led="au:red:status"
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
	preinit_regular)
		status_led_blink_preinit_regular
		;;
	done)
		status_led_on
		;;
	esac
}
