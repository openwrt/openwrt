#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "Guest Wifi" "$@"
}

do_radio24() {
	local config=$1
	local channel

	config_get channel $1 channel
	if [ $channel -gt 15 ]; then
		uci set guestwifi.$NAME.radio5g="1"
	fi
}

NAME=$1

uci set guestwifi.$NAME=guestwifi
uci set guestwifi.$NAME.ssid="Guest"
uci set guestwifi.$NAME.freq="0"
uci set guestwifi.$NAME.enabled="0"
uci set guestwifi.$NAME.encrypted=""
uci set guestwifi.$NAME.password=""
uci set guestwifi.$NAME.qos="0"
uci set guestwifi.$NAME.ul=""
uci set guestwifi.$NAME.dl=""
uci set guestwifi.$NAME.radio5g="0"
config_load wireless
config_foreach do_radio24 wifi-device
uci commit guestwifi