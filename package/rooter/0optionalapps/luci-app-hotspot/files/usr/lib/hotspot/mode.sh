#!/bin/sh

SET=$1

rm -f /tmp/hotman
uci set travelmate.global.trm_auto=$SET 
uci commit travelmate

if [ $SET = "1" ]; then
	result=`ps | grep -i "travelmate.sh" | grep -v "grep" | wc -l`
	if [ $result -ge 1 ]
   	then
		wifilog "HOTSPOT" "Travelmate already running"
	else
		/usr/lib/hotspot/travelmate.sh &
	fi
else
	hkillall travelmate.sh
fi