#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	logger -t "MDM9215 Data" "$@"
}

CURRMODEM=$1
COMMPORT=$2

OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "mdm9215info.gcom" "$CURRMODEM" | tr 'a-z' 'A-Z')
O=$($ROOTER/common/processat.sh "$OX")
O=$(echo $O)

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

Oup=$(echo $O | tr 'a-z' 'A-Z')

CSQ=$(echo $O | grep -o "CSQ: [0-9]\+" | grep -o "[0-9]\+")
[ "x$CSQ" = "x" ] && CSQ=-1

if [ $CSQ -ge 0 -a $CSQ -le 31 ]; then
    CSQ_PER=$(($CSQ * 100/31))
    CSQ_RSSI=$((2 * CSQ - 113))
    CSQX=$CSQ_RSSI
    [ $CSQ -eq 0 ] && CSQ_RSSI="<= "$CSQ_RSSI
    [ $CSQ -eq 31 ] && CSQ_RSSI=">= "$CSQ_RSSI
    CSQ_PER=$CSQ_PER"%"
    CSQ_RSSI=$CSQ_RSSI" dBm"
else
    CSQ="-"
    CSQ_PER="-"
    CSQ_RSSI="-"
fi

MODE="-"
WS46=$(echo $O" " | grep -o "+COPS: .\+ OK " | tr " " ",")
TECH=$(echo $WS46 | cut -d, -f5)

if [ ! -z "$TECH" ]; then
	MODE=$TECH
	SGCELL=$(echo $O" " | grep -o "\$QCSQ .\+ OK " | tr " " "," | tr ",:" ",")

	WS46=$(echo $O" " | grep -o "AT\$QCSYSMODE? .\+ OK ")
	WS46=$(echo "$WS46" | sed -e "s/AT\$QCSYSMODE? //g")
	WS46=$(echo "$WS46" | sed -e "s/ OK//g")

	case $MODE in
		*)
			RSCP=$(echo $SGCELL | cut -d, -f8)
			RSCP="-"$(echo $RSCP | grep -o "[0-9]\{1,3\}")
			ECIO=$(echo $SGCELL| cut -d, -f5)
			ECIO="-"$(echo $ECIO | grep -o "[0-9]\{1,3\}")
			RSSI=$(echo $SGCELL | cut -d, -f4)
			CSQ_RSSI="-"$(echo $RSSI | grep -o "[0-9]\{1,3\}")" dBm"
			;;
		"7")
			RSSI=$(echo $SGCELL | cut -d, -f4)
			CSQ_RSSI=$(echo $RSSI | grep -o "[0-9]\{1,3\}")" dBm"
			RSCP=$(echo $SGCELL | cut -d, -f8)
			RSCP="-"$(echo $RSCP | grep -o "[0-9]\{1,3\}")
			ECIO=$(echo $SGCELL| cut -d, -f4)
			ECIO="-"$(echo $ECIO | grep -o "[0-9]\{1,3\}")
			;;
	esac

	MODE=$WS46
fi

NETMODE="1"
MODTYPE="8"

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
