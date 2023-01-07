#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "Guest Wifi" "$@"
}

NAME=$1
