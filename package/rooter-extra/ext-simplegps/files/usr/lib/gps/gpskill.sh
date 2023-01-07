#!/bin/sh

CURRMODEM=$1
ROOTER=/usr/lib/rooter
CPORT=$(uci get modem.modem$CURRMODEM.commport)
PID=$(ps | grep "[q]uectel.sh $CURRMODEM" | awk '{print $1}')
if [ -n "$PID" ]; then
	kill -9 $PID
	ATCMDD="AT+QGPSEND"
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	rm -f /tmp/gps
	rm -f /tmp/gpsdata
	rm -f /tmp/gpsdata1
fi
PID=$(ps | grep "[s]ierra.sh $CURRMODEM" | awk '{print $1}')
if [ -n "$PID" ]; then
	kill -9 $PID
	ATCMDD="AT!GPSEND=0"
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	rm -f /tmp/gps
	rm -f /tmp/gpsdata
	rm -f /tmp/gpsdata1
fi
