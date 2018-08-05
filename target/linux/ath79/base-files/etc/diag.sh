#!/bin/sh

. /lib/functions.sh
. /lib/functions/leds.sh

board=$(board_name)

status_led="$(get_dt_led status)"

set_state() {
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
		case "$board" in
		ligowawe,dlb5-propeller)
			active_fw=$(fw_printenv -n active)
                	fw_setenv linux_fail${active_fw}=0
		;;
		esac
		;;
	esac
}
