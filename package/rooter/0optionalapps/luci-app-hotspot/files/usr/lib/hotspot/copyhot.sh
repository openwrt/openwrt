#!/bin/sh

if [ -f "/tmp/hot" ]; then
	rm -f /etc/hotspot
	while IFS='|' read -r ssid encrypt key flag
	do
		echo $ssid"|"$encrypt"|"$key >> /etc/hotspot
	done <"/tmp/hot"
fi
