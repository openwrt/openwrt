#!/bin/sh

ROOTER=/usr/lib/rooter

CURRMODEM=$1
COMMPORT=$2

lte_bw() {
	BW=$(echo $BW | grep -o "[0-5]\{1\}")
	case $BW in
		"0")
			BW="1.4" ;;
		"1")
			BW="3" ;;
		"2"|"3"|"4"|"5")
			BW=$((($(echo $BW) - 1) * 5)) ;;
	esac
}

nr_bw() {
	BW=$(echo $BW | grep -o "[0-9]\{1,2\}")
	case $BW in
		"0"|"1"|"2"|"3"|"4"|"5")
			BW=$((($(echo $BW) + 1) * 5)) ;;
		"6"|"7"|"8"|"9"|"10"|"11"|"12")
			BW=$((($(echo $BW) - 2) * 10)) ;;
		"13")
			BW="200" ;;
		"14")
			BW="400" ;;
	esac
}

OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "quectelinfo.gcom" "$CURRMODEM")

OX=$(echo $OX | tr 'a-z' 'A-Z')

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
PCI="-"
CTEMP="-"
SINR="-"
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
NR_NSA=$(echo $OX | grep -o "+QENG:[ ]\?\"NR5G-NSA\",")
NR_SA=$(echo $OX | grep -o "+QENG: \"SERVINGCELL\",[^,]\+,\"NR5G-SA\",\"[DFT]\{3\}\",")
if [ -n "$NR_NSA" ]; then
	QENG=",,"$(echo $OX" " | grep -o "+QENG: \"LTE\".\+\"NR5G-NSA\"," | tr " " ",")
	QENG5=$(echo $OX | grep -o "+QENG:[ ]\?\"NR5G-NSA\",[0-9]\{3\},[0-9]\{2,3\},[0-9]\{1,5\},-[0-9]\{2,5\},[-0-9]\{1,3\},-[0-9]\{2,3\},[0-9]\{1,7\},[0-9]\{1,3\}.\{1,6\}")
	if [ -z "$QENG5" ]; then
		QENG5=$(echo $OX | grep -o "+QENG:[ ]\?\"NR5G-NSA\",[0-9]\{3\},[0-9]\{2,3\},[0-9]\{1,5\},-[0-9]\{2,3\},[-0-9]\{1,3\},-[0-9]\{2,3\}")
		if [ -n "$QENG5" ]; then
			QENG5=$QENG5",,"
		fi
	fi
elif [ -n "$NR_SA" ]; then
	QENG=$(echo $NR_SA | tr " " ",")
	QENG5=$(echo $OX | grep -o "+QENG: \"SERVINGCELL\",[^,]\+,\"NR5G-SA\",\"[DFT]\{3\}\",[ 0-9]\{3,4\},[0-9]\{2,3\},[0-9A-F]\{1,10\},[0-9]\{1,5\},[0-9A-F]\{2,6\},[0-9]\{6,7\},[0-9]\{1,3\},[0-9]\{1,2\},-[0-9]\{2,5\},-[0-9]\{2,3\},[-0-9]\{1,3\}")
else
	QENG=$(echo $OX" " | grep -o "+QENG: [^ ]\+ " | tr " " ",")
fi
QCA=$(echo $OX" " | grep -o "+QCAINFO: \"S[CS]\{2\}\".\+NWSCANMODE" | tr " " ",")
QNSM=$(echo $OX | grep -o "+QCFG: \"NWSCANMODE\",[0-9]")
QNWP=$(echo $OX | grep -o "+QNWPREFCFG: \"MODE_PREF\",[A-Z5:]\+" | cut -d, -f2)
QTEMP=$(echo $OX | grep -o "+QTEMP: [0-9]\{1,3\}")
if [ -z "$QTEMP" ]; then
	QTEMP=$(echo $OX | grep -o "+QTEMP:[ ]\?\"XO[_-]THERM[_-][^,]\+,[\"]\?[0-9]\{1,3\}" | grep -o "[0-9]\{1,3\}")
fi
if [ -z "$QTEMP" ]; then
	QTEMP=$(echo $OX | grep -o "+QTEMP:[ ]\?\"MDM-CORE-USR.\+[0-9]\{1,3\}\"" | cut -d\" -f4)
fi
if [ -z "$QTEMP" ]; then
	QTEMP=$(echo $OX | grep -o "+QTEMP:[ ]\?\"MDMSS.\+[0-9]\{1,3\}\"" | cut -d\" -f4)
fi
if [ -n "$QTEMP" ]; then
	CTEMP=$(echo $QTEMP | grep -o "[0-9]\{1,3\}")$(printf "\xc2\xb0")"C"
fi
RAT=$(echo $QENG | cut -d, -f4 | grep -o "[-A-Z5]\{3,7\}")
rm -f /tmp/modnetwork
case $RAT in
	"GSM")
		MODE="GSM"
		;;
	"WCDMA")
		MODE="WCDMA"
		CHANNEL=$(echo $QENG | cut -d, -f9)
		RSCP=$(echo $QENG | cut -d, -f12)
		RSCP="-"$(echo $RSCP | grep -o "[0-9]\{1,3\}")
		ECIO=$(echo $QENG | cut -d, -f13)
		ECIO="-"$(echo $ECIO | grep -o "[0-9]\{1,3\}")
		;;
	"LTE"|"CAT-M"|"CAT-NB")
		MODE=$(echo $QENG | cut -d, -f5 | grep -o "[DFT]\{3\}")
		if [ -n "$MODE" ]; then
			MODE="$RAT $MODE"
		else
			MODE="$RAT"
		fi
		PCI=$(echo $QENG | cut -d, -f9)
		CHANNEL=$(echo $QENG | cut -d, -f10)
		LBAND=$(echo $QENG | cut -d, -f11 | grep -o "[0-9]\{1,3\}")
		BW=$(echo $QENG | cut -d, -f12)
		lte_bw
		BWU=$BW
		BW=$(echo $QENG | cut -d, -f13)
		lte_bw
		BWD=$BW
		if [ -z "$BWD" ]; then
			BWD="unknown"
		fi
		if [ -z "$BWU" ]; then
			BWU="unknown"
		fi
		if [ -n "$LBAND" ]; then
			LBAND="B"$LBAND" (Bandwidth $BWD MHz Down | $BWU MHz Up)"
		fi
		RSRP=$(echo $QENG | cut -d, -f15 | grep -o "[0-9]\{1,3\}")
		if [ -n "$RSRP" ]; then
			RSCP="-"$RSRP
			RSRPLTE=$RSCP
		fi
		RSRQ=$(echo $QENG | cut -d, -f16 | grep -o "[0-9]\{1,3\}")
		if [ -n "$RSRQ" ]; then
			ECIO="-"$RSRQ
		fi
		RSSI=$(echo $QENG | cut -d, -f17 | grep -o "\-[0-9]\{1,3\}")
		if [ -n "$RSSI" ]; then
			CSQ_RSSI=$RSSI" dBm"
		fi
		SINRR=$(echo $QENG | cut -d, -f18 | grep -o "[0-9]\{1,3\}")
		if [ -n "$SINRR" ]; then
			if [ $SINRR -le 25 ]; then
				SINR=$((($(echo $SINRR) * 2) -20))" dB"
			fi
		fi

		if [ -n "$NR_NSA" ]; then
			MODE="LTE/NR EN-DC"
			echo "0" > /tmp/modnetwork
			if [ -n "$QENG5" ]  && [ -n "$LBAND" ] && [ "$RSCP" != "-" ] && [ "$ECIO" != "-" ]; then
				PCI="$PCI, "$(echo $QENG5 | cut -d, -f4)
				SCHV=$(echo $QENG5 | cut -d, -f8)
				SLBV=$(echo $QENG5 | cut -d, -f9)
				BW=$(echo $QENG5 | cut -d, -f10 | grep -o "[0-9]\{1,3\}")
				if [ -n "$SLBV" ]; then
					LBAND=$LBAND"<br />n"$SLBV
					if [ -n "$BW" ]; then
						nr_bw
						LBAND=$LBAND" (Bandwidth $BW MHz)"
					fi
					if [ "$SCHV" -ge 123400 ]; then
						CHANNEL=$CHANNEL", "$SCHV
					else
						CHANNEL=$CHANNEL", -"
					fi
				else
					LBAND=$LBAND"<br />nxx (unknown NR5G band)"
					CHANNEL=$CHANNEL", -"
				fi
				RSCP=$RSCP" dBm<br />"$(echo $QENG5 | cut -d, -f5)
				SINRR=$(echo $QENG5 | cut -d, -f6 | grep -o "[0-9]\{1,3\}")
				if [ -n "$SINRR" ]; then
					if [ $SINRR -le 30 ]; then
						SINR=$SINR"<br />"$((($(echo $SINRR) * 2) -20))" dB"
					fi
				fi
				ECIO=$ECIO" (4G) dB<br />"$(echo $QENG5 | cut -d, -f7)" (5G) "
			fi
		fi
		if [ -z "$LBAND" ]; then
			LBAND="-"
		else
			if [ -n "$QCA" ]; then
				QCA=$(echo $QCA | grep -o "\"S[CS]\{2\}\"[-0-9A-Z,\"]\+")
				for QCAL in $(echo "$QCA"); do
					if [ $(echo "$QCAL" | cut -d, -f7) = "2" ]; then
						SCHV=$(echo $QCAL | cut -d, -f2 | grep -o "[0-9]\+")
						SRATP="B"
						if [ -n "$SCHV" ]; then
							CHANNEL="$CHANNEL, $SCHV"
							if [ "$SCHV" -gt 123400 ]; then
								SRATP="n"
							fi
						fi
						SLBV=$(echo $QCAL | cut -d, -f6 | grep -o "[0-9]\{1,2\}")
						if [ -n "$SLBV" ]; then
							LBAND=$LBAND"<br />"$SRATP$SLBV
							BWD=$(echo $QCAL | cut -d, -f3 | grep -o "[0-9]\{1,3\}")
							if [ -n "$BWD" ]; then
								UPDOWN=$(echo $QCAL | cut -d, -f13)
								case "$UPDOWN" in
									"UL" )
										CATYPE="CA"$(printf "\xe2\x86\x91") ;;
									"DL" )
										CATYPE="CA"$(printf "\xe2\x86\x93") ;;
									* )
										CATYPE="CA" ;;
								esac
								if [ $BWD -gt 14 ]; then
									LBAND=$LBAND" ("$CATYPE", Bandwidth "$(($(echo $BWD) / 5))" MHz)"
								else
									LBAND=$LBAND" ("$CATYPE", Bandwidth 1.4 MHz)"
								fi
							fi
							LBAND=$LBAND
						fi
						PCI="$PCI, "$(echo $QCAL | cut -d, -f8)
					fi
				done
			fi
		fi
		if [ $RAT = "CAT-M" ] || [ $RAT = "CAT-NB" ]; then
			LBAND="B$(echo $QENG | cut -d, -f11) ($RAT)"
		fi
		;;
	"NR5G-SA")
		MODE="NR5G-SA"
		echo "0" > /tmp/modnetwork
		if [ -n "$QENG5" ]; then
			MODE="$RAT $(echo $QENG5 | cut -d, -f4)"
			PCI=$(echo $QENG5 | cut -d, -f8)
			CHANNEL=$(echo $QENG5 | cut -d, -f10)
			LBAND=$(echo $QENG5 | cut -d, -f11)
			BW=$(echo $QENG5 | cut -d, -f12)
			nr_bw
			LBAND="n"$LBAND" (Bandwidth $BW MHz)"
			RSCP=$(echo $QENG5 | cut -d, -f13)
			ECIO=$(echo $QENG5 | cut -d, -f14)
			if [ "$CSQ_PER" = "-" ]; then
				RSSI=$(rsrp2rssi $RSCP $BW)
				CSQ_PER=$((100 - (($RSSI + 51) * 100/-62)))"%"
				CSQ=$((($RSSI + 113) / 2))
				CSQ_RSSI=$RSSI" dBm"
			fi
			SINRR=$(echo $QENG5 | cut -d, -f15 | grep -o "[0-9]\{1,3\}")
			if [ -n "$SINRR" ]; then
				if [ $SINRR -le 30 ]; then
					SINR=$((($(echo $SINRR) * 2) -20))" dB"
				fi
			fi
		fi
		;;
esac

QRSRP=$(echo "$OX" | grep -o "+QRSRP:[^,]\+,-[0-9]\{1,5\},-[0-9]\{1,5\},-[0-9]\{1,5\}[^ ]*")
if [ -n "$QRSRP" ] && [ "$RAT" != "WCDMA" ]; then
	QRSRP1=$(echo $QRSRP | cut -d, -f1 | grep -o "[-0-9]\+")
	QRSRP2=$(echo $QRSRP | cut -d, -f2)
	QRSRP3=$(echo $QRSRP | cut -d, -f3)
	QRSRP4=$(echo $QRSRP | cut -d, -f4)
	QRSRPtype=$(echo $QRSRP | cut -d, -f5)
	if [ "$QRSRPtype" == "NR5G" ]; then
		if [ -n "$NR_SA" ]; then
			RSCP=$QRSRP1
			if [ -n "$QRSRP2" -a "$QRSRP2" != "-32768" ]; then
				RSCP1="RxD "$QRSRP2
			fi
			if [ -n "$QRSRP3" -a "$QRSRP3" != "-32768" -a "$QRSRP3" != "-44" ]; then
				RSCP=$RSCP" dBm<br />"$QRSRP3
			fi
			if [ -n "$QRSRP4" -a "$QRSRP4" != "-32768" -a "$QRSRP4" != "-44" ]; then
				RSCP1="RxD "$QRSRP4
			fi
		else
			RSCP=$RSRPLTE
			if [ -n "$QRSRP1" -a "$QRSRP1" != "-32768" ]; then
				RSCP=$RSCP" (4G) dBm<br />"$QRSRP1
				if [ -n "$QRSRP2" -a "$QRSRP2" != "-32768" ]; then
					RSCP="$RSCP,$QRSRP2"
					if [ -n "$QRSRP3" -a "$QRSRP3" != "-32768" ]; then
						RSCP="$RSCP,$QRSRP3"
						if [ -n "$QRSRP4" -a "$QRSRP4" != "-32768" ]; then
							RSCP="$RSCP,$QRSRP4"
						fi
					fi
					RSCP=$RSCP" (5G) "
				fi
			fi
		fi
	elif [ "$QRSRP2$QRSRP3$QRSRP4" != "-44-44-44" -a -z "$QENG5" ]; then
		RSCP=$QRSRP1
		if [ "$QRSRP3$QRSRP4" == "-140-140" -o "$QRSRP3$QRSRP4" == "-44-44" -o "$QRSRP3$QRSRP4" == "-32768-32768" ]; then
			RSCP1="RxD "$(echo $QRSRP | cut -d, -f2)
		else
			RSCP=$RSCP" dBm (RxD "$QRSRP2" dBm)<br />"$QRSRP3
			RSCP1="RxD "$QRSRP4
		fi
	fi
fi

QNSM=$(echo "$QNSM" | grep -o "[0-9]")
if [ -n "$QNSM" ]; then
	MODTYPE="6"
	case $QNSM in
	"0" )
		NETMODE="1" ;;
	"1" )
		NETMODE="3" ;;
	"2"|"5" )
		NETMODE="5" ;;
	"3" )
		NETMODE="7" ;;
	esac
fi
if [ -n "$QNWP" ]; then
	MODTYPE="6"
	case $QNWP in
	"AUTO" )
		NETMODE="1" ;;
	"WCDMA" )
		NETMODE="5" ;;
	"LTE" )
		NETMODE="7" ;;
	"LTE:NR5G" )
		NETMODE="8" ;;
	"NR5G" )
		NETMODE="9" ;;
	esac
fi

CMODE=$(uci -q get modem.modem$CURRMODEM.cmode)
if [ "$CMODE" = 0 ]; then
	NETMODE="10"
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
	echo 'TEMP="'"$CTEMP"'"'
	echo 'SINR="'"$SINR"'"'
}  > /tmp/signal$CURRMODEM.file

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
