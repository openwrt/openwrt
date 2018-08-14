#!/bin/sh

. /lib/functions/leds.sh

status_led="$(get_dt_led status)"
running="$(get_dt_led running)"

set_state() {
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
		[ -n "$running" ] && {
			status_led_off
			status_led="$running"
		}
		status_led_on
		;;
	esac
}
