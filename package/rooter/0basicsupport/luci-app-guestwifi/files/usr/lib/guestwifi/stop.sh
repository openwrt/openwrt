#!/bin/sh

log() {
	logger -t "Guest Wifi" "$@"
}

NAME=$1

uci delete wireless.$NAME
uci commit wireless
QOS=$(uci get guestwifi.$NAME.qos)
if [ $QOS = "1" ]; then
	uci delete sqm.$NAME
	uci commit sqm
	/etc/init.d/sqm start
	/etc/init.d/sqm enable
fi
uci set guestwifi.$NAME.enabled="0"
uci commit guestwifi