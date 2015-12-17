#!/bin/sh
# Copyright (C) 2012-2013 OpenWrt.org

. /lib/functions/leds.sh

status_led=power
[ ! -d /sys/class/leds/power/ ] && [ ! -d /sys/class/leds/power1/ ] && [ ! -d /sys/class/leds/power2/ ] && [ -d /sys/class/leds/wps/ ] && status_led=wps

set_state() {
	case "$1" in
	preinit)
		if [ -d /sys/class/leds/power2/ ]; then
			status_led_on
			status_led=power2
			status_led_blink_preinit
			status_led=power
		else
			status_led_blink_preinit
		fi
		;;
	failsafe)
		if [ -d /sys/class/leds/power2/ ]; then
			led_off power2
			status_led_blink_failsafe
		elif [ -d /sys/class/leds/power1/ ]; then
			status_led_off
			status_led=power1
			status_led_blink_failsafe
			status_led=power
		else
			status_led_blink_failsafe
		fi
		;;
	preinit_regular)
		if [ -d /sys/class/leds/power2/ ]; then
			status_led_on
			status_led=power2
			status_led_blink_preinit_regular
			status_led=power
		else
			status_led_blink_preinit_regular
		fi
		;;
	done)
		status_led_on
		led_off power1
		led_off power2
		;;
	esac
}
