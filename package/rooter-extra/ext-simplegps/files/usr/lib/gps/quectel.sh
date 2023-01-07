#!/bin/sh

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
	logger -t "Quectel GPS" "$@"
}

ifname1="ifname"
if [ -e /etc/newstyle ]; then
	ifname1="device"
fi

CURRMODEM=$1
if [ $CURRMODEM = "2" ]; then
	exit 0
fi
CPORT=$(uci get modem.modem$CURRMODEM.commport)
rm -f /tmp/gps
rm -f /tmp/lastgps
if [ -z "$CPORT" ]; then
	exit 0
fi

ATCMDD="AT+QGPS?"
OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
err=$(echo "$OX" | grep "+QGPS: 1")
if [ -z "$err" ]; then
	ATCMDD="AT+QGPS=1"
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
fi

log "GPS setup and waiting"

ATCMDD="AT+QCFG=\"gpsdrx\""
OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
err=$(echo "$OX" | grep "0")
if [ -n "$err" ]; then
	ATCMDD="AT+QCFG=\"gpsdrx\",1"
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
fi
ATCMDD="AT+QGPSCFG=\"outport\",\"none\""
OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")

while true; do
	refresh=30
	ATCMDD="AT+QGPSLOC=0"
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	err=$(echo "$OX" | grep "ERROR")
	if [ -z "$err" ]; then
		echo "$OX" > /tmp/gpsox
		result=`ps | grep -i "processq.sh" | grep -v "grep" | wc -l`
		if [ $result -lt 1 ]; then
			/usr/lib/gps/processq.sh 1
		fi
		sleep $refresh
	else
		sleep 5
	fi
done
