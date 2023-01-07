#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	logger -t "ublox Data" "$@"
}

CURRMODEM=$1
COMMPORT=$2

get_ublox() {
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "ubloxinfo.gcom" "$CURRMODEM" | tr 'a-z' 'A-Z')
}

get_ublox

UCGED=$(echo $OX | grep -o "+UCGED: 2")
if [ -z "$UCGED" ]; then
	ATCMDD="AT+UCGED=2"
	UCGED=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	get_ublox
fi
OX=$(echo $OX)

RSRP=""
RSRQ=""
CHANNEL="-"
ECIO="-"
RSCP="-"
ECIO1=" "
RSCP1=" "
MODE="-"
MODTYPE="-"
NETMODE="-"
LBAND="-"
TEMP="-"
PCI="-"
SINR="-"

CSQ=$(echo $OX | grep -o "+CSQ: .\+ +CESQ" | tr " " ",")
CESQ=$(echo $OX | grep -o "+CESQ: .\+ +URAT" | tr " " ",")
URAT=$(echo $OX | grep -o "+URAT: .\+ +UCGED" | tr " " ",")
UCGED=$(echo $OX" " | grep -o "+UCGED: .\+ OK " | tr " " ",")

CSQ=$(echo $CSQ | cut -d, -f2)
CSQ=$(echo $CSQ | grep -o "[0-9]\{1,2\}")

if [ "$CSQ" -eq "99" ]; then
	CSQ=""
fi
if [ -n "$CSQ" ]; then
	CSQ_PER=$(($CSQ * 100/31))"%"
	CSQ_RSSI=$((2 * CSQ - 113))" dBm"
else
	CSQ="-"
	CSQ_PER="-"
	CSQ_RSSI="-"
fi

RAT=$(echo $UCGED | cut -d, -f3)
case "$RAT" in
	"2")
		MODE="GSM"
		LAC=$(echo $UCGED | cut -d, -f11)
		LAC=$(echo $LAC | grep -o "[0-9A-F]\{4\}")
		CID=$(echo $UCGED | cut -d, -f9)
		CID=$(echo $CID | grep -o "[0-9A-F]\{4\}")
		;;
	"3")
		MODE="UMTS"
		CHANNEL=$(echo $UCGED | cut -d, -f7)
		LAC=$(echo $UCGED | cut -d, -f10)
		LAC=$(echo $LAC | grep -o "[0-9A-F]\{4\}")
		CID=$(echo $UCGED | cut -d, -f9)
		CID=$(echo $CID | grep -o "[0-9A-F]\{5,8\}")
		RSCP=$(echo $CESQ | cut -d, -f4)
		RSCP=$(echo $RSCP | grep -o "[0-9]\{1,3\}")
		if [ "$RSCP" -eq "255" ]; then
			RSCP=""
		fi
		if [ -n "$RSCP" ]; then
			RSCP=$(($RSCP - 121))
		fi
		ECIO=$(echo $CESQ | cut -d, -f5)
		ECIO=$(echo $ECIO | grep -o "[0-9]\{1,3\}")
		if [ "$ECIO" -eq "255" ]; then
			ECIO=""
		fi
		if [ -n "$ECIO" ]; then
			ECIO=$((($ECIO / 2) - 24))
		fi
		;;
	"4")
		MODE="LTE"
		LBAND=$(echo $UCGED | cut -d, -f8)
		if [ "$LBAND" -eq "255" ]; then
			LBAND=""
		fi
		BWU=$(echo $UCGED | cut -d, -f9)
		BWU=$(echo $BWU | grep -o "[0-9]\{1,3\}")
		BWD=$(echo $UCGED | cut -d, -f10)
		BWD=$(echo $BWD | grep -o "[0-9]\{1,3\}")
		if [ -z "$BWD" ]; then
			LBAND=""
		fi
		if [ -z "$BWU" ]; then
			LBAND=""
		fi
		if [ -z "$LBAND" ]; then
			LBAND="-"
		else
			if [ "$BWU" = "6" ]; then
				BWU="1.4"
			else
				BWU=$(($(echo $BWU) / 5))
			fi
			if [ "$BWD" = "6" ]; then
				BWD="1.4"
			else
				BWD=$(($(echo $BWD) / 5))
			fi
			LBAND="B"$LBAND" (Bandwidth $BWD MHz Down | $BWU MHz Up)"
		fi
		LAC=$(echo $UCGED | cut -d, -f11)
		LAC=$(echo $LAC | grep -o "[0-9A-F]\{4\}")
		CID=$(echo $UCGED | cut -d, -f12)
		CID=$(echo $CID | grep -o "[0-9A-F]\{5,8\}")
		RSRP=$(echo $CESQ | cut -d, -f7)
		RSRP=$(echo $RSRP | grep -o "[0-9]\{1,3\}")
		if [ "$RSRP" -eq "255" ]; then
			RSRP=""
		fi
		if [ -n "$RSRP" ]; then
			RSRP=$(($RSRP - 141))
			RSCP=$RSRP
		fi
		RSRQ=$(echo $CESQ | cut -d, -f6)
		RSRQ=$(echo $RSRQ | grep -o "[0-9]\{1,3\}")
		if [ "$RSRQ" -eq "255" ]; then
			RSRQ=""
		fi
		if [ -n "$RSRQ" ]; then
			RSRQ=$((($RSRQ / 2) - 19))
			ECIO=$RSRQ
		fi
		;;
esac

if [ $RAT -eq "2" ]; then
	if [ -n "$CID" ]; then
		CID_NUM=$(printf "%d" 0x$CID)
		CID=$CID" ("$CID_NUM")"
	fi
else
	CID=$(echo $CID | grep -o "[0-9A-F]\{5,8\}")
	if [ -n "$CID" ]; then
		LCID=$(printf "%08X" 0x$CID)
		LCID_NUM=$(printf "%d" 0x$LCID)
		if [ "$RAT" -eq "4" ]; then
			RNC=$(printf "${LCID:1:5}")
			CID=$(printf "${LCID:6:2}")
		else
			RNC=$(printf "${LCID:1:3}")
			CID=$(printf "${LCID:4:4}")
		fi
		CID_NUM=$(printf "%d" 0x$CID)
		CID=$CID" ("$CID_NUM")"
		RNC_NUM=" ("$(printf "%d" 0x$RNC)")"
	fi
fi

if [ -n "$LAC" ]; then
	LAC_NUM=$(printf "%d" 0x$LAC)
	LAC=$LAC" ("$LAC_NUM")"
else
	LAC="-"
	LAC_NUM="-"
fi

URAT1=$(echo $URAT | cut -d, -f2)
URAT2=$(echo $URAT | cut -d, -f3)
if [ -n "$URAT1" ]; then
	MODTYPE="5"
	case $URAT1 in
	"0" )
		NETMODE="3"
		;;
	"2" )
		NETMODE="5"
		;;
	"3" )
		NETMODE="7"
		;;
	* )
		case $URAT2 in
		"0" )
			NETMODE="2"
			;;
		"2" )
			NETMODE="4"
			;;
		"3" )
			NETMODE="1"
			;;
		esac
		;;
	esac
fi

echo 'CSQ="'"$CSQ"'"' > /tmp/signal$CURRMODEM.file
echo 'CSQ_PER="'"$CSQ_PER"'"' >> /tmp/signal$CURRMODEM.file
echo 'CSQ_RSSI="'"$CSQ_RSSI"'"' >> /tmp/signal$CURRMODEM.file
echo 'ECIO="'"$ECIO"'"' >> /tmp/signal$CURRMODEM.file
echo 'RSCP="'"$RSCP"'"' >> /tmp/signal$CURRMODEM.file
echo 'ECIO1="'"$ECIO1"'"' >> /tmp/signal$CURRMODEM.file
echo 'RSCP1="'"$RSCP1"'"' >> /tmp/signal$CURRMODEM.file
echo 'MODE="'"$MODE"'"' >> /tmp/signal$CURRMODEM.file
echo 'MODTYPE="'"$MODTYPE"'"' >> /tmp/signal$CURRMODEM.file
echo 'NETMODE="'"$NETMODE"'"' >> /tmp/signal$CURRMODEM.file
echo 'CHANNEL="'"$CHANNEL"'"' >> /tmp/signal$CURRMODEM.file
echo 'LBAND="'"$LBAND"'"' >> /tmp/signal$CURRMODEM.file
echo 'LAC="'"$LAC"'"' >> /tmp/signal$CURRMODEM.file
echo 'LAC_NUM="'""'"' >> /tmp/signal$CURRMODEM.file
echo 'CID="'"$CID"'"' >> /tmp/signal$CURRMODEM.file
echo 'CID_NUM="'""'"' >> /tmp/signal$CURRMODEM.file
echo 'RNC="'"$RNC"'"' >> /tmp/signal$CURRMODEM.file
echo 'RNC_NUM="'"$RNC_NUM"'"' >> /tmp/signal$CURRMODEM.file
echo 'TEMP="'"$TEMP"'"' >> /tmp/signal$CURRMODEM.file
echo 'PCI="'"$PCI"'"' >> /tmp/signal$CURRMODEM.file
echo 'SINR="'"$SINR"'"' >> /tmp/signal$CURRMODEM.file

if [ "$CSQ" = "-" ]; then
	log "$OX"
fi

WWANX=$(uci get modem.modem$CURRMODEM.interface)
OPER=$(cat /sys/class/net/$WWANX/operstate 2>/dev/null)
rm -f "/tmp/connstat"$CURRMODEM

if [ ! $OPER ]; then
	exit 0
fi
if echo $OPER | grep -q "unknown"; then
	exit 0
fi

if echo $OPER | grep -q "down"; then
	echo "1" > "/tmp/connstat"$CURRMODEM
fi
