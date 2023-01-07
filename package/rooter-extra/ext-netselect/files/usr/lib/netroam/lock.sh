#!/bin/sh

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
	logger -t "ROAM" "$@"
}

CURRMODEM=$1
CPORT=$(uci get modem.modem$CURRMODEM.commport)

if [ -e /tmp/rlock ]; then
	source /tmp/rlock
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	log "$OX"
	rm -f /tmp/rlock
fi