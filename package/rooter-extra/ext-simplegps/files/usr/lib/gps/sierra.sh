#!/bin/sh

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
	logger -t "Sierra GPS" "$@"
}

ifname1="ifname"
if [ -e /etc/newstyle ]; then
	ifname1="device"
fi

CURRMODEM=$1
CPORT=$(uci get modem.modem$CURRMODEM.commport)
rm -f /tmp/gps
rm -f /tmp/lastgps
if [ -z "$CPORT" ]; then
	exit 0
fi

ATCMDD="AT!CUSTOM?"
OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")

GPSsel=$(echo $OX | grep "GPSSEL")
GPSenable=$(echo $OX | grep "GPSENABLE")
if [ -z "$GPSsel" -o -z "$GPSenable" ]; then
	ATCMDD="AT!ENTERCND=\"A710\""
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	if [ -z "$GPSsel" ]; then
		ATCMDD="at!custom=\"GPSSEL\",1"
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	fi
	if [ -z "$GPSenable" ]; then
		ATCMDD="at!custom=\"GPSENABLE\",1"
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		ATCMDD="AT+CFUN=0;+CFUN=1,1"
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		exit
	fi
	ATCMDD="AT!ENTERCND=\"AWRONG\""
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
fi

ATCMDD="AT!GPSTRACK=1,240,30,1000,5"
OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")

while true; do
	refresh=30
	ATCMDD="at!gpsloc?"
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	err=$(echo "$OX" | grep "Not Available")
	if [ -z "$err" ]; then
		echo "$OX" > /tmp/gpsox
		result=`ps | grep -i "processs.sh" | grep -v "grep" | wc -l`
		if [ $result -lt 1 ]; then
			/usr/lib/gps/processs.sh 1
		fi
		sleep $refresh
	else
		sleep 5
	fi
done
