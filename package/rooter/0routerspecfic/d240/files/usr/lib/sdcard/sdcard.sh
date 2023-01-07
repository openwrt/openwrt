#!/bin/sh

log() {
	logger -t "sdcard" "$@"
}

ACTION=$1

if [ $ACTION = "add" ]; then
	log "add"
else
	log "remove"
fi

