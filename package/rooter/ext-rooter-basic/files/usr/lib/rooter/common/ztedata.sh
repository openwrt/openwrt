#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	logger -t "ZTE Data" "$@"
}

CURRMODEM=$1
COMMPORT=$2

fix_data() {
	O=$($ROOTER/common/processat.sh "$OY")
}

process_csq() {
	CSQ=$(echo "$O" | awk -F[,\ ] '/^\+CSQ:/ {print $2}')
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
}

process_zte() {
	ZRSSI=$(echo "$O" | awk -F[,\ ] '/^\+ZRSSI:/ {print $2}')
	if [ "x$ZRSSI" != "x" ]; then
		TMP_RSSI=$CSQ_RSSI
		CSQ_RSSI="-"$ZRSSI" dBm"
		ECI=$(echo "$O" | awk -F[,\ ] '/^\+ZRSSI:/ {print $3}')
		if [ "x$ECI" != "x" ]; then
			ECIO=`expr $ECI / 2`
			ECIO="-"$ECIO
			RSCP=$(echo "$O" | awk -F[,\ ] '/^\+ZRSSI:/ {print $4}')
			if [ "x$RSCP" != "x" ]; then
				RSCP=`expr $RSCP / 2`
				RSCP="-"$RSCP
			else
				CSQ_RSSI=$TMP_RSSI
				RSCP=$ZRSSI
				ECIO=$ECI
			fi
		else
			RSCP=$ZRSSI
			CSQ_RSSI=$TMP_RSSI
			ECIO=`expr $RSCP - $CSQX`
		fi
	fi

	MODE="-"
	TECH=$(echo "$O" | awk -F[,\ ] '/^\+ZPAS:/ {print $2}' | sed 's/"//g')
	if [ "x$TECH" != "x" -a "x$TECH" != "xNo" ]; then
		MODE="$TECH"
	fi

	ZSNT=$(echo "$O" | awk -F[,\ ] '/^\+ZSNT:/ {print $2}')
	if [ "x$ZSNT" != "x" ]; then
		MODTYPE="1"
		if [ $ZSNT = "0" ]; then
			ZSNTX=$(echo "$O" | awk -F[,\ ] '/^\+ZSNT:/ {print $4}')
			case $ZSNTX in
			"0" )
				NETMODE="1"
				;;
			"1" )
				NETMODE="2"
				;;
			"2" )
				NETMODE="4"
				;;
			"6" )
				NETMODE="6"
				;;
			esac
		else
			case $ZSNT in
			"1" )
				NETMODE="3"
				;;
			"2" )
				NETMODE="5"
				;;
			"6" )
				NETMODE="7"
				;;
			esac
		fi
	fi

	ZCELLINFO=$(echo $O | grep -o "+ZCELLINFO: .\+ OK")
	if [ -n "$ZCELLINFO" ]; then
		LBAND=$(echo $ZCELLINFO | cut -d, -f3 | grep -o "LTE B[0-9]\{1,2\}")
		CHANNEL=$(echo $ZCELLINFO | cut -d, -f4 | grep -o "[0-9]\+")
		if [ -n "$LBAND" ]; then
			LBAND="B"$(echo $LBAND | grep -o "[0-9]\+")
		else
			LBAND="-"
			CHANNEL="-"
		fi
	fi

	ZSINR=$(echo $OY | grep -o "+ZSINR: [-0-9]\{1,3\},[0-9],")
	if [ -n "$ZSINR" ]; then
		ZSINR=${ZSINR:8}
		SINR=$(echo $ZSINR | cut -d, -f1)"."$(echo $ZSINR | cut -d, -f2)" dB"
	fi

	CMODE=$(uci get modem.modem$CURRMODEM.cmode)
	if [ $CMODE = 0 ]; then
		NETMODE="10"
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
MODETYPE="-"
NETMODE="-"
LBAND="-"
CHANNEL="-"
TEMP="-"
PCI="-"
SINR="-"

OY=$($ROOTER/gcom/gcom-locked "$COMMPORT" "zteinfo.gcom" "$CURRMODEM")

fix_data
process_csq
process_zte

{
	echo 'CSQ="'"$CSQ"'"' > /tmp/signal$CURRMODEM.file
	echo 'CSQ_PER="'"$CSQ_PER"'"'
	echo 'CSQ_RSSI="'"$CSQ_RSSI"'"'
	echo 'ECIO="'"$ECIO"'"'
	echo 'RSCP="'"$RSCP"'"'
	echo 'ECIO1="'"$ECIO1"'"'
	echo 'RSCP1="'"$RSCP1"'"'
	echo 'MODE="'"$MODE"'"'
	echo 'MODTYPE="'"$MODTYPE"'"'
	echo 'NETMODE="'"$NETMODE"'"'
	echo 'LBAND="'"$LBAND"'"'
	echo 'CHANNEL="'"$CHANNEL"'"'
	echo 'TEMP="'"$TEMP"'"'
	echo 'PCI="'"$PCI"'"'
	echo 'SINR="'"$SINR"'"'
} > /tmp/signal$CURRMODEM.file

CONNECT=$(uci get modem.modem$CURRMODEM.connected)
if [ $CONNECT -eq 0 ]; then
	exit 0
fi

if [ $CSQ = "-" ]; then
	log "$OY"
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
