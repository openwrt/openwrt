#!/bin/sh
# Copyright (C) 2007-2013 OpenWrt.org

# This setup gives us 4.5 distinguishable states:
#
# (1-LED) Solid OFF:    Bootloader running, or kernel hung (timer task stalled)
# (1-LED) Solid ON:     Kernel hung (timer task stalled)
# (2-LED) Solid RED:    Bootloader running, or kernel hung (timer task stalled)
# (2-LED) Solid YELLOW: Kernel hung (timer task stalled)
# 5Hz blink:            preinit
# 10Hz blink:           failsafe
# (1-LED) Heartbeat:    normal operation
# (2-LED) Solid GREEN:  normal operation

. /lib/functions/leds.sh

get_status_led() {
	[ -d "/sys/class/leds/status" ] && status_led="status"
	[ -d "/sys/class/leds/power:green" ] && status_led="power:green"
}

set_state() {
	get_status_led

	case "$1" in
	preinit)
		status_led_set_timer 100 100
		;;
	failsafe)
		status_led_set_timer 50 50
		;;
	preinit_regular)
		status_led_blink_preinit_regular
		;;
	done)
		[ "$status_led" = "status" ] && {
			status_led_set_heartbeat
		}
		[ "$status_led" = "power:green" ] && {
			status_led_set_on
			led_off "power:red"
		}
		;;
	esac
}
