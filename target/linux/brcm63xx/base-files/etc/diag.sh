#!/bin/sh
# Copyright (C) 2007-2013 OpenWrt.org

. /lib/functions/leds.sh
. /lib/brcm63xx.sh

set_state() {
	case "$1" in
	preinit)
		status_led_set_timer 200 200
		;;
	failsafe)
		status_led_set_timer 50 50
		;;
	preinit_regular)
		status_led_set_timer 500 500
		;;
	done)
		if [ "${status_led/power}" != "$status_led" ]; then
			status_led_on
		else
			status_led_off
		fi
		;;
	esac
}
