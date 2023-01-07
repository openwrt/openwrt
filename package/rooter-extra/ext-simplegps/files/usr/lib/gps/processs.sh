#!/bin/sh

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
	logger -t "Quectel GPS" "$@"
}

OX=$1

if [ -z "$OX" ]; then
	if [ -e /tmp/lastgps ]; then
		OX=$(cat /tmp/lastgps)
	else
		exit 0
	fi
else
	OX=$(cat /tmp/gpsox)
	echo "$OX" > /tmp/lastgps
fi

O=$(echo "$OX" | grep "at!gpsloc?")
if [ -z "$O" ]; then
	exit 0
fi

CURRMODEM=1
CPORT=$(uci get modem.modem$CURRMODEM.commport)
ATCMDD="at!gpssatinfo?"
OY=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
echo "$OY" > /tmp/satgps
Y=$(cat /tmp/satgps)
OY=$(echo $Y" " | tr " " ",")


OX=$(echo $OX" " | tr " " ",")
LATD=$(echo $OX | cut -d, -f3)
LATM=$(echo $OX | cut -d, -f5)
LATS=$(echo $OX | cut -d, -f7)
LATH=$(echo $OX | cut -d, -f9)

LOND=$(echo $OX | cut -d, -f12)
LONM=$(echo $OX | cut -d, -f14)
LONS=$(echo $OX | cut -d, -f16)
LONH=$(echo $OX | cut -d, -f18)
lathemid=$LATH
lonhemid=$LONH

if [ $LATH = "S" ]; then
	LATH="South"
else
	LATH="North"
fi
if [ $LONH = "E" ]; then
	LONH="East"
else
	LONH="West"
fi
delatitude=$LATD" Deg "$LATM" Min "$LATS" Sec "$LATH
delongitude=$LOND" Deg "$LONM" Min "$LONS" Sec "$LONH
/usr/lib/gps/convert.lua $LATD $LATM $LATS $lathemid 1
source /tmp/latlon
dlatitude=$CONVERT
/usr/lib/gps/convert.lua $LOND $LONM $LONS $lonhemid 1
source /tmp/latlon
dlongitude=$CONVERT

lat="$delatitude ( $dlatitude )"
long="$delongitude ( $dlongitude )"
echo 'LATITUDE="'"$lat"'"' >> /tmp/gpsdata
echo 'LONGITUDE="'"$long"'"' >> /tmp/gpsdata

echo "$lat" > /tmp/gpsdata1
echo "$long" >> /tmp/gpsdata1

echo "0" > /tmp/gps
