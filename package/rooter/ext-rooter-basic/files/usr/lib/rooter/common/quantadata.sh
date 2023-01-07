#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	logger -t "Quanta Data" "$@"
}

CURRMODEM=$1
COMMPORT=$2

OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "quantainfo.gcom" "$CURRMODEM")

OX=$(echo $OX | tr 'a-z' 'A-Z')

RSRP=""
RSRQ=""
CHANNEL="-"
ECIO="-"
RSCP="-"
ECIO1=" "
RSCP1=" "
MODE="-"
LBAND="-"
PCI="-"
SINR="-"
TEMP="-"

CSQ=$(echo $OX | grep -o "+CSQ: [0-9]\{1,2\}" | grep -o "[0-9]\{1,2\}")
if [ "$CSQ" = "99" ]; then
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

REGX='*QRFINFO: \"LTE\",[0-9]\{1,2\},[0-9]\{1,5\},[.012345]\{1,3\},[0-9]\{1,3\},-[0-9]\{1,3\},[-0-9]\{1,3\},[.0-9]\{1,4\}'
LTE=$(echo $OX | grep -o "$REGX")
if [ -n "$LTE" ]; then
	MODE="LTE"
	BW=$(echo $LTE | cut -d, -f4)
	LBAND="B"$(echo $LTE | cut -d, -f2)"(Bandwidth "$BW" MHz)"
	RSCP=$(echo $LTE | cut -d, -f6)
	ECIO=$(echo $LTE | cut -d, -f7)
	CHANNEL=$(echo $LTE | cut -d, -f3)
	PCI=$(echo $LTE | cut -d, -f5)
	SINR=$(echo $LTE | cut -d, -f8)" dB"
else
	REGX='*QRFINFO: \"WCDMA\",[0-9]\{1,2\},[0-9]\{1,5\},[^,],[0-9]\{1,3\},-[0-9]\{1,3\},[-0-9]\{1,3\}'
	UMTS=$(echo $OX | grep -o "$REGX")
	if [ -n "$UMTS" ]; then
		MODE="WCDMA"
		LBAND="B"$(echo $UMTS | cut -d, -f2)
		RSCP=$(echo $UMTS | cut -d, -f6)
		ECIO=$(echo $UMTS | cut -d, -f7)
		CHANNEL=$(echo $UMTS | cut -d, -f3)
		PCI=$(echo $UMTS | cut -d, -f5)
	fi
fi

TECH=$(echo $OX | grep -o "\^QCNCFG: \"[0123]\{2\}\"" | grep -o "[0123]\{2\}")
case $TECH in
	"02")
		NETMODE="5"
		;;
	"03")
		NETMODE="7"
		;;
	*)
		NETMODE="1"
		;;
esac

MODTYPE="11"

{
	echo 'CSQ="'"$CSQ"'"'
	echo 'CSQ_PER="'"$CSQ_PER"'"'
	echo 'CSQ_RSSI="'"$CSQ_RSSI"'"'
	echo 'ECIO="'"$ECIO"'"'
	echo 'RSCP="'"$RSCP"'"'
	echo 'ECIO1="'"$ECIO1"'"'
	echo 'RSCP1="'"$RSCP1"'"'
	echo 'MODE="'"$MODE"'"'
	echo 'MODTYPE="'"$MODTYPE"'"'
	echo 'NETMODE="'"$NETMODE"'"'
	echo 'CHANNEL="'"$CHANNEL"'"'
	echo 'LBAND="'"$LBAND"'"'
	echo 'PCI="'"$PCI"'"'
	echo 'TEMP="'"$TEMP"'"'
	echo 'SINR="'"$SINR"'"'
} > /tmp/signal$CURRMODEM.file

CONNECT=$(uci get modem.modem$CURRMODEM.connected)
if [ $CONNECT -eq 0 ]; then
    exit 0
fi

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
