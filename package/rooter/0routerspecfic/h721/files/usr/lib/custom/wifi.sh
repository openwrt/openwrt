#!/bin/sh 
. /lib/functions.sh


do_radio() {
	local config=$1
	local channel
	mode="ap"

	config_get channel $1 channel
	if [ $channel -lt 15 ]; then
		RADIO=$config
		ifname="$(ubus -S call network.wireless status | jsonfilter -l 1 -e "@.$RADIO.interfaces[@.config.mode=\"${mode}\"].ifname")"
		if [ ! -z $ifname ]; then
			iw reg set US
			iwconfig $ifname txpower 30
		fi
	fi
}

while [ ! -e /etc/config/wireless ]
do
	sleep 1
done
sleep 3
if [ ! -e /etc/maxwifi ]; then
	config_load wireless
	config_foreach do_radio wifi-device
	echo "0" > /etc/maxwifi
fi
