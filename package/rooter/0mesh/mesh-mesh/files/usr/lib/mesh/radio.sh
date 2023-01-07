#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "Radio" "$@"
}

count_radio() {
	local config=$1
	local channel

	config_get channel $1 channel
	count=$((${count}+1))
}

do_radio() {
	local config=$1
	local channel

	config_get channel $1 channel
	rn=${config#radio}
	freq="1"
	if [ $channel -lt 15 ]; then
		freq="0"
	fi
	iwin=$(iw phy phy$rn info)
	hw=0
	hwt=$(echo "$iwin" | grep 'short GI for 40 MHz')
	if [ ! -z "$hwt" ]; then
		hw=1
	fi
	hwt=$(echo "$iwin" | grep 'short GI (80 MHz)')
	if [ ! -z "$hwt" ]; then
		hw=2
	fi
	hwt=$(echo "$iwin" | grep 'short GI (160')
	if [ ! -z "$hwt" ]; then
		hw=3
	fi
	echo "$freq|$rn|$hw" >> /tmp/radiolist
}

count=0
rm -f /tmp/radiolist
config_load wireless
config_foreach count_radio wifi-device
if [ $count -gt 0 ]; then
	echo "$count" > /tmp/radiolist
	config_foreach do_radio wifi-device
fi

CF1=$(uci -q get mesh.radio.radionumber)
CF2=$(uci -q get mesh.radio.channelindex)
CF3=$(uci -q get mesh.radio.channellist)
CF4=$(uci -q get mesh.radio.channelwidth)
CF5=$(uci -q get mesh.radio.usedfs)
CF6=$(uci -q get mesh.radio.dedicated)
echo "$CF1" >> /tmp/radiolist
echo "$CF2" >> /tmp/radiolist
echo "$CF3" >> /tmp/radiolist
echo "$CF4" >> /tmp/radiolist
echo "$CF5" >> /tmp/radiolist
echo "$CF6" >> /tmp/radiolist

CF1=$(uci -q get mesh.network.networkid)
CF2=$(uci -q get mesh.network.netencrypted)
CF3=$(uci -q get mesh.network.netpassword)
echo "$CF1" >> /tmp/radiolist
echo "$CF2" >> /tmp/radiolist
echo "$CF3" >> /tmp/radiolist

CF1=$(uci -q get mesh.roam.signalenable)
CF2=$(uci -q get mesh.roam.signalid)
echo "$CF1" >> /tmp/radiolist
echo "$CF2" >> /tmp/radiolist



