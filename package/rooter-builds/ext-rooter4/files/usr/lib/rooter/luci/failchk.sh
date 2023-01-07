#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	logger -t "Failover Check" "$@"
}

jkillall failover.sh
ENB=$(uci get failover.enabled.enabled)
if [ $ENB = "1" ]; then
	if [ -e $ROOTER/connect/failover.sh ]; then
		log "Restarting Failover System"
		$ROOTER/connect/failover.sh &
	fi
fi
