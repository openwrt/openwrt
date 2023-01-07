#!/bin/sh

ROOTER=/usr/lib/rooter

CURRMODEM=$1
ATCMDD=$2

COMMPORT="/dev/ttyUSB"$(uci get modem.modem$CURRMODEM.commport)

OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "sendsms-at.gcom" "$CURRMODEM" "$ATCMDD")
