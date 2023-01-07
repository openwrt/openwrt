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

O=$(echo "$OX" | grep "+QGPSLOC:")
if [ -z "$O" ]; then
	exit 0
fi
OX=$(echo $O" " | tr ":" ",")

LAT=$(echo $OX | cut -d, -f3)
LON=$(echo $OX | cut -d, -f4)

llen=$(expr length "$LAT")
if [ $llen -eq 10 ]; then
	LAT="0"$LAT
fi
if [ $llen -eq 9 ]; then
	LAT="00"$LAT
fi
llen=$(expr length "$LON")
if [ $llen -eq 10 ]; then
	LON="0"$LON
fi
if [ $llen -eq 9 ]; then
	LON="00"$LON
fi

latdeg=${LAT:0:3}
latmin=${LAT:3:2}
latsec=${LAT:6:4}
lathemi=${LAT:10:1}
londeg=${LON:0:3}
lonmin=${LON:3:2}
lonsec=${LON:6:4}
lonhemi=${LON:10:1}
lathemid=$lathemi
lonhemid=$lonhemi

let "latsecd=$latsec*6/1000"
let "lonsecd=$lonsec*6/1000"

latdeg="${latdeg#"${latdeg%%[!0]*}"}"
latmin="${latmin#"${latmin%%[!0]*}"}"
if [ $lathemi = "S" ]; then
	lathemi="South"
else
	lathemi="North"
fi
delatitude=$latdeg" Deg "$latmin" Min "$latsecd" Sec "$lathemi
if [ $lonhemi = "E" ]; then
	lonhemi="East"
else
	lonhemi="West"
fi
londeg="${londeg#"${londeg%%[!0]*}"}"
lonmin="${lonmin#"${lonmin%%[!0]*}"}"
delongitude=$londeg" Deg "$lonmin" Min "$lonsecd" Sec "$lonhemi
/usr/lib/gps/convert.lua $latdeg $latmin $latsec $lathemid
source /tmp/latlon
dlatitude=$CONVERT
/usr/lib/gps/convert.lua $londeg $lonmin $lonsec $lonhemid
source /tmp/latlon
dlongitude=$CONVERT

lat="$delatitude ( $dlatitude )"
long="$delongitude ( $dlongitude )"
echo 'LATITUDE="'"$lat"'"' >> /tmp/gpsdata
echo 'LONGITUDE="'"$long"'"' >> /tmp/gpsdata

echo "$lat" > /tmp/gpsdata1
echo "$long" >> /tmp/gpsdata1

echo "0" > /tmp/gps