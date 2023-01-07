#!/bin/sh
. /lib/functions.sh

config_cb() {
    local type="$1"
    local name="$2"
	if [ ! -z $type ]; then
		if [ $type = "wifi-device" ]; then
			echo $name >> /tmp/wifi-device
		fi
	fi
}

rm -f /tmp/wifi-device
config_load wireless
