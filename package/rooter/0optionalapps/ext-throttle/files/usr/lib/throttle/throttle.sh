#!/bin/sh
. /lib/functions.sh

stop_interface() {
	INTER=$1
	DEVICE=$(uci -q get sqm.$INTER.interface)
	uci set sqm.$INTER.enabled='0'
	uci commit sqm
	if [ $DEVICE != "0" ]; then
		/usr/lib/sqm/run.sh stop ${DEVICE}
	fi
}

start_interface() {
	INTER=$1
	sdown=$2
	sup=$3
	uci set sqm.$INTER.enabled='1'
	uci set sqm.$INTER.download=$sdown
	uci set sqm.$INTER.upload=$sup
	uci commit sqm
	DEVICE=$(uci -q get sqm.$INTER.interface)
	if [ $DEVICE != "0" ]; then
		/usr/lib/sqm/run.sh stop ${DEVICE}
		/usr/lib/sqm/run.sh start ${DEVICE}
	fi
}

cmd=$1
if [ $cmd = 'start' ]; then
	mult=$4
	if [ -z $mult ]; then
		down=$2"000"
		up=$3"000"
	else
		down=$2
		up=$3
	fi
	/etc/init.d/sqm enabled
	start_interface wan $down $up
	start_interface wan1 $down $up
	start_interface wan2 $down $up
else
	stop_interface wan
	stop_interface wan1
	stop_interface wan2
fi