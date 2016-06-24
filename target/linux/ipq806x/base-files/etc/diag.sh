#!/bin/sh

. /lib/functions/leds.sh
. /lib/ipq806x.sh

get_status_led() {
	case $(ipq806x_board_name) in
	c2600)
		status_led="status:blue"
		;;
	ea8500)
		status_led="ea8500:white:power"
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
