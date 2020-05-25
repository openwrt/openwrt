#!/bin/sh

. /lib/functions/leds.sh

get_status_led_platform() {
	local status_led_file

	# There may be more than one color of power LED, try to avoid amber/red
	status_led_file=$(find /sys/class/leds/ -name "*:power" -a ! -name "*:amber:*" -a ! -name "*:red:*" | head -n1)
	if [ -d "$status_led_file" ]; then
		status_led=$(basename $status_led_file)
		return
	fi;

	# Now just pick any power LED
	status_led_file=$(find /sys/class/leds/ -name "*:power" | head -n1)
	if [ -d "$status_led_file" ]; then
		status_led=$(basename $status_led_file)
		return
	fi;

	# And finally, let's also try the device-Tree aliases node
	status_led="$(get_dt_led status)"

	boot="$status_led"
	failsafe="$status_led"
	running="$status_led"
	upgrade="$status_led"
}
