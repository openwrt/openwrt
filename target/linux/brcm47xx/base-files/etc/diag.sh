#!/bin/sh
# Copyright (C) 2006 OpenWrt.org

. /lib/functions/leds.sh

get_status_led() {
	status_led_file=$(find /sys/class/leds/ -name "*power*" |head -n1)
	if [ ! -f $status_led_file ]; then
		status_led=$(basename $status_led_file)
		return
	fi;
	status_led_file=$(find /sys/class/leds/ -name "*diag*" |head -n1)
	if [ ! -f $status_led_file ]; then
		status_led=$(basename $status_led_file)
		return
	fi;
	status_led_file=$(find /sys/class/leds/ -name "*wps*" |head -n1)
	if [ ! -f $status_led_file ]; then
		status_led=$(basename $status_led_file)
		return
	fi;
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
