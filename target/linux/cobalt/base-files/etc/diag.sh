#!/bin/sh
# Copyright (C) 2009-2013 OpenWrt.org

. /lib/functions/leds.sh

status_led="qube::front"

set_state() {
	case "$1" in
	preinit)
		status_led_on
		;;
	done)
		status_led_off
		;;
	esac
}
