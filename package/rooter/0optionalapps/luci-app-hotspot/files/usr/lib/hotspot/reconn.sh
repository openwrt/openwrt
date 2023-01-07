#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "Reconnect" "$@"
}

reconn=$1
log "$reconn"
uci set travelmate.global.reconn=$reconn
uci commit travelmate