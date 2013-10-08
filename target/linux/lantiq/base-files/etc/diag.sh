#!/bin/sh
# Copyright (C) 2012-2013 OpenWrt.org

. /lib/functions/leds.sh

status_led="power"

set_state() {
	[ -d /sys/class/leds/power2/ ] && {

		case "$1" in
		preinit)
			led_set_attr "power2" "trigger" "heartbeat"
			status_led_on
			;;
		failsafe)
			led_off "power2"
			status_led_set_timer 100 100
			;;
		done)
			led_off "power2"
			;;
		esac
		return
	}

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
