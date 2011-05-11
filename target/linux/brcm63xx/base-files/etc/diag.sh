#!/bin/sh
#
# Copyright (C) 2007 OpenWrt.org
#
#

. /lib/brcm63xx.sh

led_set_attr() {
	[ -f "/sys/class/leds/$1/$2" ] && echo "$3" > "/sys/class/leds/$1/$2"
}

status_led_set_timer() {
	led_set_attr $status_led "trigger" "timer"
	led_set_attr $status_led "delay_on" "$1"
	led_set_attr $status_led "delay_off" "$2"
	[ -n "$status_led2" ] && {
		led_set_attr $status_led2 "trigger" "timer"
		led_set_attr $status_led2 "delay_on" "$1"
		led_set_attr $status_led2 "delay_off" "$2"
	}
}

status_led_set_morse() {
	led_set_attr $status_led "trigger" "morse"
	led_set_attr $status_led "delay" "$1"
	led_set_attr $status_led "message" "$2"
	[ -n "$status_led2" ] && {
		led_set_attr $status_led2 "trigger" "morse"
		led_set_attr $status_led2 "delay" "$1"
		led_set_attr $status_led2 "message" "$2"
	}
}

status_led_on() {
	led_set_attr $status_led "trigger" "none"
	led_set_attr $status_led "brightness" 255
	[ -n "$status_led2" ] && {
		led_set_attr $status_led2 "trigger" "none"
		led_set_attr $status_led2 "brightness" 255
	}
}

status_led_off() {
	led_set_attr $status_led "trigger" "none"
	led_set_attr $status_led "brightness" 0
	[ -n "$status_led2" ] && {
		led_set_attr $status_led2 "trigger" "none"
		led_set_attr $status_led2 "brightness" 0
	}
}

set_state() {
	case "$1" in
	preinit)
		insmod leds-gpio
		status_led_on
		status_led_set_timer 200 200
		;;
	failsafe)
		status_led_set_timer 50 50
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
