#!/bin/sh
#
# This script is called by dsl_cpe_control whenever there is a DSL event
# and calls any available hotplug script(s) in /etc/hotplug.d/dsl.

# remembered for /etc/init.d/dsl_led
[ "$DSL_NOTIFICATION_TYPE" = "DSL_INTERFACE_STATUS" ] &&
	echo "$DSL_INTERFACE_STATUS" > /var/run/dsl_interface_status

exec /sbin/hotplug-call dsl
