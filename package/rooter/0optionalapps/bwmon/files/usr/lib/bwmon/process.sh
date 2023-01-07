#!/bin/sh

log() {
	logger -t "BWmon Process" "$@"
}

running=0
if [ -e "/tmp/WRTbmon" ]; then
	running=1
fi

sleep 5

if [ $running = 0 ]; then
		log "Enable BandWidthMonitor"
		/usr/lib/bwmon/wrtbwmon.sh &
fi



