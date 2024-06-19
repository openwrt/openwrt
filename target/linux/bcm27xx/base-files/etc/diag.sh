#!/bin/sh
# Copyright (C) 2015-2016 OpenWrt.org
# Copyright (C) 2017 LEDE project

. /lib/functions.sh
. /lib/functions/leds.sh

set_state() {
	if [ -d "/sys/class/leds/PWR" ]; then
		status_led="PWR"
	else
		status_led="ACT"
	fi

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
                status_led_blink_preinit_regular
                ;;
	done)
		status_led_on
		;;
	esac
}
