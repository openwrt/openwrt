#!/bin/sh

log() {
	logger -t "NITZ set time" "$@"
}

CURRMODEM=$(uci -q get modem.general.modemnum)
COMMPORT="/dev/ttyUSB"$(uci -q get modem.modem$CURRMODEM.commport)
ROOTER=/usr/lib/rooter

ATCMDD="AT+CTZU?;+CCLK?"
OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
OX=$OX
SUCCESS=1
NITZstat=$(echo "$OX" | grep "+CTZU: 1")
if [ -n "$NITZstat" ]; then
	DTG=$(echo "$OX" | grep -o "+CCLK:.*" | cut -d\" -f2)
	DTGyymmdd=$(echo "$DTG" | cut -d, -f1 | tr '/' '-')
	DTGhhmmss=$(echo "$DTG" | cut -d, -f2)
	DTGhhmmss=${DTGhhmmss:0:8}
	DTGtz=$(echo "$DTG" | grep -o "[-+][0-9]\{1,2\}")
	if [ -n "$DTGtz" ]; then
		DTGif=$(date +%s -u -s "$DTGyymmdd $DTGhhmmss")
		OX=$(date -s @$DTGif)
		log "$OX"
		SUCCESS=0
	fi
fi
exit $SUCCESS
