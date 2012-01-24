#!/bin/sh
#
# This script is called by dsl_cpe_control whenever there is a DSL event,
# we only actually care about the DSL_INTERFACE_STATUS events as these
# tell us the line has either come up or gone down.
#
# The rest of the code is basically the same at the atm hotplug code
#

[ "$DSL_NOTIFICATION_TYPE" = "DSL_INTERFACE_STATUS" ] || exit 0

. /etc/functions.sh

include /lib/network
scan_interfaces

local found=0
local ifc
for ifc in $interfaces; do
	local up
	config_get_bool up "$ifc" up 0

	local auto
	config_get_bool auto "$ifc" auto 1

	local proto
	config_get proto "$ifc" proto

	if [ "$DSL_INTERFACE_STATUS" = "UP" ]; then
		if [ "$proto" = "pppoa" ] && [ "$up" != 1 ] && [ "$auto" = 1 ]; then
			found=1
			( sleep 1; ifup "$ifc" ) &
		fi
	else
		if [ "$proto" = "pppoa" ] && [ "$up" = 1 ] && [ "$auto" = 1 ]; then
			found=1
			( sleep 1; ifdown "$ifc" ) &
		fi
	fi
done

if [ "$found" != 1 ]; then
	logger "Found no matching interface for DSL notification ($DSL_INTERFACE_STATUS)"
fi
