#!/bin/sh

log() {
	logger -t "wrtbwmon" "$@"
}

enable=$1

WX=$(uci -q get custom.bwallocate.lock)
if [ "$WX" = "1" ]; then
	enable="1"
fi
uci set bwmon.general.enabled=$enable
uci commit bwmon
