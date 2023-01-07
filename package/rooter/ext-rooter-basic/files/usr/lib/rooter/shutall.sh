#!/bin/sh

ROOTER=/usr/lib/rooter
CURRMODEM=1
COMMPORT=$(uci get modem.modem$CURRMODEM.commport)
if [ -n "$COMMPORT" ]; then
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$COMMPORT" "run-at.gcom" "$CURRMODEM" "AT+CFUN=0")
fi
CURRMODEM=2
COMMPORT=$(uci get modem.modem$CURRMODEM.commport)
if [ -n "$COMMPORT" ]; then
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$COMMPORT" "run-at.gcom" "$CURRMODEM" "AT+CFUN=0")
fi
poweroff
