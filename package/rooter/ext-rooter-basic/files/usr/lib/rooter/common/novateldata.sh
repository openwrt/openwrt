#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	logger -t "Novatel Data" "$@"
}

CURRMODEM=$1
COMMPORT=$2

fix_data() {
	OY=$(echo $OY | tr 'a-z' 'A-Z')
	O=$($ROOTER/common/processat.sh "$OY")
	O=$(echo $O" ")
	O=$(echo "${O//[\"]/}")
}

process_csq() {
CSQ=$(echo $OY | grep -o "+CSQ: [0-9]\{1,2\}" | grep -o "[0-9]\{1,2\}")
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
}

CSQ="-"
CSQ_PER="-"
CSQ_RSSI="-"
ECIO="-"
RSCP="-"
ECIO1=" "
RSCP1=" "
MODE="-"
MODTYPE="-"
NETMODE="-"
LBAND="-"
CHANNEL="-"
TEMP="-"
PCI="-"
SINR="-"

OY=$($ROOTER/gcom/gcom-locked "$COMMPORT" "novatelinfo.gcom" "$CURRMODEM")

fix_data
process_csq

DEG=$(echo $O" " | grep -o "+NWDEGC: .\+ OK " | tr " " ",")
TMP=$(echo $DEG | cut -d, -f2)
if [ -n "$TMP" ]; then
	TEMP=$TMP$(printf "\xc2\xb0")"C"
fi

MODE="-"
PSRAT=$(echo $O" " | grep -o "\$NWRAT: .\+ OK " | tr " " ",")
TECH=$(echo $PSRAT | cut -d, -f4)
if [ -n "$TECH" ]; then
	case "$TECH" in
		"1"|"2"|"3")
			MODE="UMTS"
			;;
		"4"|"5"|"6")
			MODE="GSM"
			;;
		"7"|"8"|"9")
			MODE="LTE"
			VZWRSRP=$(echo "$O" | grep -o "VZWRSRP: [0-9]\{1,3\},[0-9]\{1,7\},[-.0-9]\{1,7\}" | tr " " ",")
			PCI=$(echo "$VZWRSRP" | cut -d, -f2)
			CHANNEL=$(echo "$VZWRSRP" | cut -d, -f3)
			LBAND=$("$ROOTER/chan2band.sh" "$CHANNEL")
			TMP=$(echo "$VZWRSRP" | cut -d, -f4)
			if [ -n "$TMP" ]; then
				RSCP=$TMP
			fi
			VZWRSRQ=$(echo "$O" | grep -o "VZWRSRQ: [0-9]\{1,3\},[0-9]\{1,7\},[-.0-9]\{1,7\}" | tr " " ",")
			TMP=$(echo "$VZWRSRQ" | cut -d, -f4)
			if [ -n "$TMP" ]; then
				ECIO=$TMP
			fi
			;;
		*)
			MODE="CDMA/HDR"
			;;
	esac
fi

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
	echo 'TEMP="'"$TEMP"'"'
	echo 'PCI="'"$PCI"'"'
	echo 'SINR="'"$SINR"'"'
} > /tmp/signal$CURRMODEM.file

CONNECT=$(uci get modem.modem$CURRMODEM.connected)
if [ $CONNECT -eq 0 ]; then
	exit 0
fi

ENB="0"
if [ -e /etc/config/failover ]; then
	ENB=$(uci get failover.enabled.enabled)
fi
if [ $ENB = "1" ]; then
	exit 0
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
