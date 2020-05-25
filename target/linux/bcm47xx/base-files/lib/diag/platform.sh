#!/bin/sh
# Copyright (C) 2006 OpenWrt.org

. /lib/functions/leds.sh

get_status_led_platform() {
	for led in dmz power diag wps; do
		status_led_file=$(find /sys/class/leds/ -name "*${led}*" | head -n1)
		if [ ! -f $status_led_file ]; then
			status_led=$(basename $status_led_file)
			break
		fi;
	done

	boot="$status_led"
	failsafe="$status_led"
	running="$status_led"
	upgrade="$status_led"
}
