#!/bin/sh 

BAND=$1
if [ $BAND = "1" ]; then
	BAND=2
else
	BAND=5
fi

/usr/lib/hotspot/enable.sh 0

uci set travelmate.global.freq=$BAND
uci set travelmate.global.ssid="6"
uci commit travelmate

result=`ps | grep -i "travelmate.sh" | grep -v "grep" | wc -l`
if [ $result -ge 1 ]
then
	wifilog "Band Change" "Travelmate already running"
else
	/usr/lib/hotspot/travelmate.sh &
fi
uci set travelmate.global.ssid="Hotspot Manager Interface"
uci commit travelmate
exit 0