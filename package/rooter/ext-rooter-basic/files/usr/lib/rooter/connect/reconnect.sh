#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	modlog "Reconnect Modem $CURRMODEM" "$@"
}

CURRMODEM=$1
log "Re-starting Connection for Modem $CURRMODEM"
$ROOTER/luci/restart.sh $CURRMODEM 11

