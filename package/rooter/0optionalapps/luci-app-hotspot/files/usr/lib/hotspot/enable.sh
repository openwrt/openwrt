#!/bin/sh
. /lib/functions.sh

SET=$1

uci set travelmate.global.trm_enabled=$SET
uci commit travelmate

wif=$(uci -q get travelmate.global.freq)

if [ $SET = "1" ]; then
	wifilog "Enable" "Connect Hotspot"
	AU=$(uci get travelmate.global.trm_auto)
	hkillall travelmate.sh
	if [ $AU = "1" ]; then
		uci set travelmate.global.ssid="8"
		uci set travelmate.global.bssid=""
		uci set travelmate.global.state=''
		uci set travelmate.global.key=''
		uci commit travelmate
		uci -q set wireless.wwan$wif.encryption="none"
		uci -q set wireless.wwan$wif.key=
		uci set wireless.wwan$wif.ssid="Hotspot Manager Interface"
       	uci -q commit wireless
		/usr/lib/hotspot/travelmate.sh &
	fi
else
	wifilog "Enable" "Disconnect Hotspot"
	hkillall travelmate.sh
	rm -f /tmp/hotman
	uci set travelmate.global.ssid="7"
	uci set travelmate.global.state="0"
	uci set travelmate.global.key=''
	uci set travelmate.global.bssid=""
	uci set travelmate.global.trm_enabled="0"
	uci commit travelmate
	uci -q set wireless.wwan$wif.disabled=1
	uci set wireless.wwan$wif.ssid="Hotspot Manager Interface"
	uci -q commit wireless
	ubus call network.interface.wwan down
    ubus call network reload
fi