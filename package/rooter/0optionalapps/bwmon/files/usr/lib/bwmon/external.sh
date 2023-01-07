#!/bin/sh

log() {
	logger -t "wrtbwmon" "$@"
}

external=$1

ext=$(uci -q get bwmon.general.external)

if [ "$ext" = "$external" ]; then
	exit 0
else
	uci set bwmon.general.external=$external
	uci commit bwmon
	PID=$(ps |grep "wrtbwmon.sh" | grep -v grep |head -n 1 | awk '{print $1}')
	PID1=$(ps |grep "create.sh" | grep -v grep |head -n 1 | awk '{print $1}')
	if [ ! -z "$PID" ]; then
		kill -9 $PID
		kill -9 $PID1
	fi
	/usr/lib/bwmon/wrtbwmon.sh &
	/usr/lib/bwmon/create.sh &
fi