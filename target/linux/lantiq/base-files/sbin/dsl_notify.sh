#!/bin/sh
#
# This script is called by dsl_cpe_control whenever there is a DSL event,
# we only actually care about the DSL_INTERFACE_STATUS events as these
# tell us the line has either come up or gone down.
#
# The rest of the code is basically the same at the atm hotplug code
#

[ "$DSL_NOTIFICATION_TYPE" = "DSL_INTERFACE_STATUS" ] || exit 0

. /usr/share/libubox/jshn.sh
. /lib/functions.sh
. /lib/functions/leds.sh

include /lib/network
scan_interfaces

local led
config_load system
config_get led led_dsl sysfs
if [ -n "$led" ]; then
	case "$DSL_INTERFACE_STATUS" in
	  "HANDSHAKE")  led_timer $led 500 500;;
	  "TRAINING")   led_timer $led 200 200;;
	  "UP")		led_on $led;;
	  *)		led_off $led
	esac
fi

local interfaces=`ubus list network.interface.\* | cut -d"." -f3`
local ifc
for ifc in $interfaces; do

	local up
	json_load "$(ifstatus $ifc)"
	json_get_var up up

	local auto
	config_get_bool auto "$ifc" auto 1

	local proto
	json_get_var proto proto

	if [ "$DSL_INTERFACE_STATUS" = "UP" ]; then
		if [ "$proto" = "pppoa" ] && [ "$up" != 1 ] && [ "$auto" = 1 ]; then
			( sleep 1; ifup "$ifc" ) &
		fi
	else
		if [ "$proto" = "pppoa" ] && [ "$up" = 1 ] && [ "$auto" = 1 ]; then
			( sleep 1; ifdown "$ifc" ) &
		else
			json_get_var autostart autostart
			if [ "$proto" = "pppoa" ] && [ "$up" != 1 ] && [ "$autostart" = 1 ]; then
				( sleep 1; ifdown "$ifc" ) &
			fi
		fi
	fi
done


