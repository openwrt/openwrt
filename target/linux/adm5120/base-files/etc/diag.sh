#!/bin/sh
# Copyright (C) 2007-2013 OpenWrt.org

. /lib/functions/leds.sh
. /lib/adm5120.sh

set_state() {
	case "$1" in
	preinit)
		insmod leds-gpio
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
