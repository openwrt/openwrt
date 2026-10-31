#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only

. /lib/functions.sh
. /lib/functions/leds.sh

get_status_led() {
	case $(board_name) in
	hardkernel,odroid-u2|\
	hardkernel,odroid-u3|\
	hardkernel,odroid-x2)
		status_led="blue:heartbeat"
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
	upgrade)
		status_led_set_timer 200 200
		;;
	done)
		status_led_on
		;;
	esac
}
