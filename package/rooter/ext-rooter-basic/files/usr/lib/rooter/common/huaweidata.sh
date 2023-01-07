#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	logger -t "Huawei Data" "$@"
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

process_huawei() {
	CSNR=$(echo "$O" | awk -F[,\ ] '/^\^CSNR:/ {print $2}')
	if [ "x$CSNR" != "x" ]; then
		RSCP=$CSNR
		CSNR=$(echo "$O" | awk -F[,\ ] '/^\^CSNR:/ {print $3}')
		if [ "x$CSNR" != "x" ]; then
			ECIO=$CSNR
		else
			ECIO=`expr $RSCP - $CSQX`
		fi
	else
		EC=$(echo "$O" | awk -F[,\ ] '/^\+CSQ:/ {print $4}')
		if [ "x$EC" != "x" ]; then
			ECIO=$EC
			EX=$(printf %.0f $ECIO)
			RSCP=`expr $CSQX + $EX`
		fi
	fi

	HCSQ=$(echo "$O" | grep -o "\^HCSQ:[ ]*\"LTE\",[^,]*,[^,]*,[0-9]\{1,3\}")
	if [ -n "$HCSQ" ]; then
		SINR=$(echo $HCSQ | cut -d, -f4)
		if [ $SINR -lt 255 ]; then
			SINR=$((($SINR / 5) - 20))
		else
			SINR="-"
		fi
	fi

	LTERSRP=$(echo "$O" | awk -F[,\ ] '/^\^LTERSRP:/ {print $2}')
	if [ "x$LTERSRP" != "x" ]; then
		RSCP=$LTERSRP
		LTERSRP=$(echo "$O" | awk -F[,\ ] '/^\^LTERSRP:/ {print $3}')
		if [ "x$LTERSRP" != "x" ]; then
			ECIO=$LTERSRP
		else
			ECIO=`expr $RSCP - $CSQX`
		fi
	fi
	LBANDS=$(echo $O | grep -o "\^HFREQINFO:[0-9,]\+")
	LBAND=""
	CHANNEL=""
	printf '%s\n' "$LBANDS" | while read LBANDL; do
		CHANN=$(echo $LBANDL | cut -d, -f4 | grep -o "[0-9]\{2,6\}")
		if [ -z "$CHANN" ]; then
			LBAND=""
			CHANNEL=""
		else
			BWU=$(($(echo $LBANDL | cut -d, -f9) / 1000))
			BWD=$(($(echo $LBANDL | cut -d, -f6) / 1000))
			LBANDL=$(echo $LBANDL | cut -d, -f3)
			if [ -z "$LBANDL" ]; then
				LBAND=""
			else
				if [ -n "$LBAND" ]; then
					LBAND=$LBAND" aggregated with:<br />"
				fi
				LBAND=$LBAND"B"$LBANDL" (Bandwidth $BWD MHz Down | $BWU MHz Up)"
			fi
			if [ -n "$CHANN" ]; then
				if [ -n "$CHANNEL" ]; then
					CHANNEL=$CHANNEL", "$CHANN
				else
					CHANNEL=$CHANN
				fi
			fi

		fi
		{
			echo "$LBAND"
			echo "$CHANNEL"
		} > /tmp/lbandvar$CURRMODEM
	done
	if [ -e /tmp/lbandvar$CURRMODEM ]; then
		{
			read LBAND
			read CHANNEL
		} < /tmp/lbandvar$CURRMODEM
		rm /tmp/lbandvar$CURRMODEM
	fi
	if [ -z "$LBAND" ]; then
		LBAND="-"
	fi
	if [ -z "$CHANNEL" ]; then
		CHANNEL="-"
	fi

	NETMODE="0"
	SYSCFG=$(echo "$O" | awk -F[,\"] '/^\^SYSCFGEX:/ {print $2}')
	if [ "x$SYSCFG" != "x" ]; then
		MODTYPE="3"
		case $SYSCFG in
		"00" )
			NETMODE="1"
			;;
		"01" )
			NETMODE="3"
			;;
		"02" )
			NETMODE="5"
			;;
		"03" )
			NETMODE="7"
			;;
		* )
			ACQ=${SYSCFG:0:2}
			case $ACQ in
			"01" )
				NETMODE="2"
				;;
			"02" )
				NETMODE="4"
				;;
			"03" )
				NETMODE="6"
				;;
			esac
			;;
		esac
	else
		SYSCFG=$(echo "$O" | awk -F[,\ ] '/^\^SYSCFG:/ {print $2}')
		if [ "x$SYSCFG" != "x" ]; then
			MODTYPE="4"
			case $SYSCFG in
			"7" )
				NETMODE="1"
				;;
			"13" )
				NETMODE="3"
				;;
			"14" )
				NETMODE="5"
				;;
			* )
				SYSCFG=$(echo "$O" | awk -F[,\ ] '/^\^SYSCFG:/ {print $3}')
				case $SYSCFG in
				"0" )
					NETMODE="1"
					;;
				"1" )
					NETMODE="2"
					;;
				"2" )
					NETMODE="4"
					;;
				esac
				;;
			esac
		fi
	fi

	NETMODEx=$(uci get modem.modem$CURRMODEM.netmode)
	if [ "$NETMODE" != "$NETMODEx" ]; then
		uci set modem.modem$CURRMODEM.netmode="$NETMODE"
		uci commit modem
	fi

	MODE="-"
	TECH=$(echo "$O" | awk -F[,] '/^\^SYSINFOEX:/ {print $9}' | sed 's/"//g')
	if [ "x$TECH" != "x" ]; then
		MODE="$TECH"
	fi

	if [ "x$MODE" = "x-" ]; then
		TECH=$(echo "$O" | awk -F[,\ ] '/^\^SYSINFO:/ {print $8}')
		if [ "x$TECH" != "x" ]; then
			case $TECH in
				17*) MODE="HSPA+ (64QAM)";;
				18*) MODE="HSPA+ (MIMO)";;
				1*) MODE="GSM";;
				2*) MODE="GPRS";;
				3*) MODE="EDGE";;
				4*) MODE="WCDMA";;
				5*) MODE="HSDPA";;
				6*) MODE="HSUPA";;
				7*) MODE="HSPA";;
				9*) MODE="HSPA+";;
				 *) MODE=$TECH;;
			esac
		fi
	fi

	TEMP=$(echo "$O" | awk -F[,] '/^\^CHIPTEMP:/ {print $2}')
	if [ "x$TEMP" != "x" ]; then
		TEMP=$((TEMP / 10))$(printf "\xc2\xb0")"C"
	else
		TEMP="unknown"
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
CHANNEL="-"
LBAND="-"
MODETYPE="-"
NETMODE="-"
TEMP="-"
PCI="-"
SINR="-"

OY=$($ROOTER/gcom/gcom-locked "$COMMPORT" "huaweiinfo.gcom" "$CURRMODEM")

fix_data
process_csq
process_huawei

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
