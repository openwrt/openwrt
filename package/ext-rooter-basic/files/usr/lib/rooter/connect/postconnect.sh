#!/bin/sh 

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
	modlog "PostConnect $CURRMODEM" "$@"
}

CURRMODEM=$1
idV=$(uci -q get modem.modem$CURRMODEM.idV)
idP=$(uci -q get modem.modem$CURRMODEM.idP)
CPORT=$(uci get modem.modem$CURRMODEM.commport)

log "Running PostConnect script"

if [ -e /usr/lib/scan/emailchk.sh ]; then
	/usr/lib/scan/emailchk.sh &
fi