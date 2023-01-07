#!/bin/sh
. /lib/functions.sh

ROOTER=/usr/lib/rooter

log() {
	logger -t "Restart" "$@"
}

sleep 3

CURRMODEM=1
CPORT=$(uci -q get modem.modem$CURRMODEM.commport)

$ROOTER/luci/restart.sh $CURRMODEM 11