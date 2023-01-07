#!/bin/sh

log() {
	logger -t "save total" "$@"
}

total=$1

uci set custom.bwday.bwday="$total"
uci commit custom