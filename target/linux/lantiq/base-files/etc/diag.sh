#!/bin/sh
# Copyright (C) 2012-2013 OpenWrt.org

. /lib/functions/leds.sh
. /lib/functions/lantiq.sh

power="$(lantiq_get_dt_led_chosen power)"
power1="$(lantiq_get_dt_led_chosen power1)"
power2="$(lantiq_get_dt_led_chosen power2)"

status_led="$power"

set_state() {
	case "$1" in
	preinit)
		if [ -n "$power2" ]; then
			status_led_on
			status_led="$power2"
			status_led_blink_preinit
			status_led="$power"
		else
			status_led_blink_preinit
		fi
		;;
	failsafe)
		if [ -n "$power2" ]; then
			led_off "$power2"
			status_led_blink_failsafe
		elif [ -n "$power1" ]; then
			status_led_off
			status_led="$power1"
			status_led_blink_failsafe
			status_led="$power"
		else
			status_led_blink_failsafe
		fi
		;;
	preinit_regular)
		if [ -n "$power2" ]; then
			status_led_on
			status_led="$power2"
			status_led_blink_preinit_regular
			status_led="$power"
		else
			status_led_blink_preinit_regular
		fi
		;;
	done)
		status_led_on
		led_off "$power1"
		led_off "$power2"
		;;
	esac
}
