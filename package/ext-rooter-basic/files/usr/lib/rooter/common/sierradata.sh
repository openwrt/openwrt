#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	logger -t "Sierra Data" "$@"
}

read_ssc() {
	SLBAND=$(echo $OX | grep -o "LTE "$SSCx" BAND: B[0-9]\+ LTE "$SSCx" BW :")
	SLBAND=$(echo $SLBAND | grep -o " BAND: B[0-9]\+ ")
	if [ -n "$SLBAND" ]; then
		SLBAND=$(echo $SLBAND | grep -o "[0-9]\+")
		SLBAND=$(printf "<br />B%d" $SLBAND)
		BWD=$(echo $OX | grep -o " LTE "$SSCx" BW : [.012345]\+ [ML]")
		BWD=$(echo $BWD | grep -o " BW : [.012345]\+" | grep -o "[.012345]\+")
		if [ -n "$BWD" ]; then
			SLBAND=$SLBAND$(printf " (CA, Bandwidth %s MHz)" $BWD)
		else
			SLBAND=$SLBAND$(printf " (CA, Bandwidth unknown)")
		fi
		LBAND=$LBAND$SLBAND
		XTRACHAN=$(echo $OX | grep -o " LTE "$SSCx" CHAN: [0-9]\+")
		XTRACHAN=$(echo "$XTRACHAN" | grep -o "[0-9]\{2,6\}")
		if [ -n "$XTRACHAN" ]; then
			CHANNEL=$(echo "$CHANNEL", "$XTRACHAN")
		fi
	fi
}

CURRMODEM=$1
COMMPORT=$2

OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "sierrainfo.gcom" "$CURRMODEM")

OX=$(echo $OX | tr 'a-z' 'A-Z')

CSQ=$(echo $OX | grep -o "+CSQ: [0-9]\{1,2\}" | grep -o "[0-9]\{1,2\}")
if [ -z "$CSQ" ] || [ "$CSQ" = "99" ]; then
	CSQ="-"
	CSQ_PER="-"
	CSQ_RSSI="-"
else
	CSQ_PER=$(($CSQ * 100/31))"%"
	CSQ_RSSI=$((2 * CSQ - 113))" dBm"
fi

MODTYPE="-"
NETMODE="-"
LBAND="-"
PCI="-"

LTEINFO=$(echo $OX | grep -o "!LTEINFO: .\+ INTRAFREQ:" | tr " " ",")
LTEINFO=$(echo "$LTEINFO" | grep -o "[0-9]\{1,6\}\,[0-9]\{3\}\,.\+")
if [ -n "$LTEINFO" ]; then
	PCI=$(echo $LTEINFO | cut -d, -f10 | grep -o "[0-9]\{1,3\}")
	if [ -z "$PCI" ]; then
		PCI="-"
	fi
fi

SINR=$(echo $OX | grep -o "SINR (.B): [-.0-9]\{1,5\}" | tr '\n' ' ' | cut -d' ' -f3)
if [ -n "$SINR" ]; then
	SINR5=$(echo $OX | grep -o "NR5G SINR (.B): [-]*[.0-9]\{1,5\}" | cut -d' ' -f4)
	if [ -n "$SINR5" ]; then
		SINR=$SINR" dB, "$SINR5
	fi
	SINR=$SINR" dB"
else
	SINR="-"
fi
TEMP=$(echo $OX | grep -o "TEMPERATURE: [-.0-9]\+ " | grep -o "[-.0-9]\+" | tr '\n' ' ')
if [ -n "$TEMP" ]; then
	TEMP=$(printf "$(echo $TEMP | cut -d' ' -f2)\xc2\xb0C")
else
	TEMP="unknown"
fi

RAT=$(echo $OX | grep -o "SYSTEM MODE: [^ ]\{3,10\}" | cut -d' ' -f3)

MODE=$RAT
case $RAT in
	"WCDMA")
		MODE=$(echo $OX | grep -o " \*CNTI: 0,[^ ]\+" | cut -d, -f2)
		if [ -z "$MODE" ] || [ "$MODE" = "NONE" ]; then
			MODE=$RAT
		fi
		CHANNEL=$(echo $OX | grep -o "WCDMA CHANNEL: [0-9]\{3,5\}" | cut -d' ' -f3)
		if [ -n "$CHANNEL" ]; then
			LOCKCHAN=$(echo $OX | grep -o "!UMTSCHAN? ENABLE: 01 CHANNEL: [0-9]\{3,5\}")
			if [ -n "$LOCKCHAN" ]; then
				LOCKCHAN=$(echo $LOCKCHAN | grep -o "CHANNEL: [0-9]\+" | grep -o "[0-9]\+")
				CHANNEL=$CHANNEL" (Soft locked to $LOCKCHAN)"
			fi
		else
			CHANNEL="-"
		fi
		ECIO=$(echo $OX | grep -o "EC/IO: [+-]\?[.0-9]\+")
		ECIO=$(echo "$ECIO")
		ECIO1=$(echo "$ECIO" | cut -d' ' -f4)
		ECIO=$(echo "$ECIO" | cut -d' ' -f2)
		RSCP=$(echo $OX | grep -o "RSCP: -[.0-9]\+")
		RSCP=$(echo "$RSCP")
		RSCP1=$(echo "$RSCP" | cut -d' ' -f4)
		RSCP=$(echo "$RSCP" | cut -d' ' -f2)
		;;
	"LTE")
		LBAND=$(echo $OX | grep -o "LTE BAND: B[0-9]\+ LTE BW: [.012345]\+ MHZ" | grep -o "[.0-9]\+")
		LBAND=$(printf "B%d (Bandwidth %s MHz)" $LBAND)
		CHANNEL=$(echo $OX | grep -o "LTE RX CHAN: [0-9]\{1,6\}" | grep -o "[0-9]\+")
		SLBAND=$(echo $OX | grep -o " ACTIVE LTE SCELL BAND:[ ]*B[0-9]\+ LTE SCELL BW:[ ]*[.012345]\+ MHZ")
		if [ -n "$SLBAND" ]; then
			SLBAND=$(echo $SLBAND | grep -o "[.0-9]\+")
			SLBAND=$(printf "<br />B%d (CA, Bandwidth %s MHz)" $SLBAND)
			LBAND=$LBAND$SLBAND
			XTRACHAN=$(echo $OX | grep -o " LTE SCELL CHAN:[0-9]\+" | grep -o "[0-9]\{2,6\}")
			CHANNEL=$CHANNEL", "$XTRACHAN
		fi
		SSCLIST=$(echo $OX | grep -o "LTE S[CS]C[0-9] STATE:[ ]\?ACTIVE" | grep -o "[0-9]")
		for SSCVAL in $(echo "$SSCLIST"); do
			SSCx="S[CS]C"$SSCVAL
			read_ssc
		done
		if [ -n "$LTEINFO" ]; then
			RSCP=$(echo $LTEINFO | cut -d, -f12 | grep -o "[-][.0-9]\{2,5\}")
			ECIO=$(echo $LTEINFO | cut -d, -f11 | grep -o "[-.0-9]\{1,5\}")
		fi
		if [ -z "$RSCP" ]; then
			RSCP=$(echo $OX | grep -o "PCC RXM RSRP: -[0-9]\{2,3\} " | cut -d' ' -f4)
		fi
		if [ -n "$RSCP" ]; then
			RSCP1=$(echo $OX | grep -o "PCC RXD RSRP: -[0-9]\{2,3\} " | cut -d' ' -f4)
			if [ -z "$RSCP1" ]; then
				RSCP1=$(echo $OX | grep -o "PCC RXD RSSI: -[0-9]\+ RSRP (DBM): -[0-9]\{2,3\} " | cut -d' ' -f7)
			fi
		else
			RSCP=$(echo $OX | grep -o "RSRP (DBM): -[0-9]\{2,3\} " | tr '\n' ' ' | cut -d' ' -f3)
		fi
		if [ -z "$ECIO" ]; then
			ECIO=$(echo $OX | grep -o "RSRQ (DB): [-.0-9]\{1,5\} " | cut -d' ' -f3)
		fi
		;;
	"ENDC")
		MODE="LTE/NR EN-DC"
		LBAND=$(echo $OX | grep -o "LTE BAND: B[0-9]\+ LTE BW: [.012345]\+ MHZ" | grep -o "[.0-9]\+")
		LBAND=$(printf "B%d (Bandwidth %s MHz)" $LBAND)
		CHANNEL=$(echo $OX | grep -o "LTE RX CHAN: [0-9]\{1,6\}" | grep -o "[0-9]\+")
		NBAND=$(echo $OX | grep -o "NR5G BAND: N[0-9]\+ NR5G BW: [0-9]\+ MHZ")
		NBAND=$(echo "$NBAND" | cut -d' ' -f3)" "$(echo "$NBAND" | cut -d' ' -f6)
		if [ "$NBAND" != " " ]; then
			NBAND=$(echo $NBAND | grep -o "[.0-9]\+")
			NBAND=$(printf "<br />n%d (Bandwidth %s MHz)" $NBAND)
			LBAND=$LBAND$NBAND
			NCHAN=$(echo $OX | grep -o "NR5G RX CHAN: [0-9]\{6\}" | cut -d' ' -f4)
			CHANNEL=$CHANNEL", "$NCHAN
		fi
		SSCLIST=$(echo $OX | grep -o "LTE S[CS]C[0-9] STATE:[ ]\?ACTIVE" | grep -o "[0-9]")
		for SSCVAL in $(echo "$SSCLIST"); do
			SSCx="S[CS]C"$SSCVAL
			read_ssc
		done
		RSCP=$(echo $OX | grep -o "PCC RXM RSRP: -[0-9]\{2,3\} " | cut -d' ' -f4)
		RSCP1=$(echo $OX | grep -o "NR5G RSRP (DBM): -[0-9]\{2,3\} " | cut -d' ' -f4)
		ECIO=$(echo $OX | grep -o "RSRQ (DB): [-.0-9]\{1,5\} " | tr '\n' ' ' | cut -d' ' -f3)
		ECIO1=$(echo $OX | grep -o "NR5G RSRQ (DB): [-.0-9]\{1,5\} " | cut -d' ' -f4)
		;;
	"NR5G")
		MODE="NR5G-SA"
		NBAND=$(echo $OX | grep -o "NR5G BAND: N[0-9]\+ NR5G BW: [0-9]\+ MHZ")
		NBAND=$(echo "$NBAND" | cut -d' ' -f3)" "$(echo "$NBAND" | cut -d' ' -f6)
		CHANNEL=$(echo $OX | grep -o "NR5G RX CHAN: [0-9]\{1,6\}" | grep -o "[0-9]\+")
		RSCP=$(echo $OX | grep -o "PCC RXM RSRP: -[0-9]\{2,3\} " | cut -d' ' -f4)
		ECIO=$(echo $OX | grep -o "RSRQ (DB): [-.0-9]\{1,5\} " | cut -d' ' -f3)
		;;
esac

SELRAT=$(echo $OX | grep -o "!SELRAT:[^0-9]\+[0-9]\{2\}" | grep -o "[0-9]\{2\}")
if [ -n "$SELRAT" ]; then
	MODTYPE="2"
	case $SELRAT in
	"01" )
		NETMODE="5"
		;;
	"02" )
		NETMODE="3"
		;;
	"06" )
		NETMODE="7"
		;;
	* )
		NETMODE="1"
		;;
	esac
fi

CMODE=$(uci -q get modem.modem$CURRMODEM.cmode)

if [ "$CMODE" = 0 ]; then
	NETMODE="10"
fi
if [ -z "$RSCP1" ]; then
	RSCP1=" "
fi
if [ -z "$ECIO1" ]; then
	ECIO1=" "
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
	echo 'PCI="'"$PCI"'"'
	echo 'TEMP="'"$TEMP"'"'
	echo 'SINR="'"$SINR"'"'
} > /tmp/signal$CURRMODEM.file

CONNECT=$(uci -q get modem.modem$CURRMODEM.connected)

if [ "$CONNECT" -eq 0 ]; then
	exit 0
fi

if [ $CSQ = "-" ]; then
	log "$OX"
fi

ENB="0"
if [ -e /etc/config/failover ]; then
	ENB=$(uci get failover.enabled.enabled)
fi

if [ $ENB = "1" ]; then
	exit 0
fi

WWANX=$(uci -q get modem.modem$CURRMODEM.interface)
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
