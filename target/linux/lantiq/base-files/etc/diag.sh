#!/bin/sh
# Copyright (C) 2012 OpenWrt.org

[ -d /sys/class/leds/power/ ] || return

set_state() {
	[ -d /sys/class/leds/power1/ ] && {

		case "$1" in
		preinit)
			echo heartbeat >/sys/class/leds/power/trigger
			;;
		failsafe)
			echo none >/sys/class/leds/power/trigger
			echo timer >/sys/class/leds/power1/trigger
			echo 100 >/sys/class/leds/power1/delay_on
			echo 100 >/sys/class/leds/power1/delay_off
			;;
		done)
			echo none >/sys/class/leds/power/trigger
			echo none >/sys/class/leds/power1/trigger
			echo 1 >/sys/class/leds/power/brightness
			;;
		esac
		return
	}

	case "$1" in
	preinit)
		echo heartbeat >/sys/class/leds/power/trigger
		;;
	failsafe)
		echo timer >/sys/class/leds/power/trigger
		echo 100 >/sys/class/leds/power/delay_on
		echo 100 >/sys/class/leds/power/delay_off
		;;
	done)
		echo none >/sys/class/leds/power/trigger
		echo 1 >/sys/class/leds/power/brightness
		;;
	esac
}
