#!/bin/sh

log() {
	logger -t "Rollover" "$@"
}

amount=$1

uci set custom.bwallocate.rollover=$amount
uci set custom.bwallocate.persent="0"
uci commit custom
