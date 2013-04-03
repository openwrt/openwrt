#!/bin/sh
# Copyright (C) 2012-2013 OpenWrt.org

. /lib/functions/leds.sh

status_led="power"

set_state() {
	case "$1" in
	preinit)
		status_led_set_heartbeat
		;;
	failsafe)
		[ -d /sys/class/leds/power1 ] && {
			status_led_off
			led_timer "power1" 100 100
		} || status_led_set_timer 100 100
		;;
	done)
		status_led_on
		led_off "power1"
		;;
	esac
}
