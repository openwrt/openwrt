#!/bin/sh

ROOTER=/usr/lib/rooter

CURRMODEM=$1
COMMPORT=$2

log() {
	logger -t "Telit Data" "$@"
}
decode_bw() {
	case $BW in
		"0")
			BW="1.4"
			;;
		"1")
			BW="3"
			;;
		"2")
			BW="5"
			;;
		"3")
			BW="10"
			;;
		"4")
			BW="15"
			;;
		"5")
			BW="20"
			;;
		*)
			BW=""
			;;
	esac
}
decode_band() {
	if [ "$SLBV" -lt 134 ]; then
		SLBV=$(($SLBV - 119))
	elif [ "$SLBV" -eq 134 ]; then
		SLBV="17"
	elif [ "$SLBV" -lt 143 ]; then
		SLBV=$(($SLBV - 102))
	elif [ "$SLBV" -lt 147 ]; then
		SLBV=$(($SLBV - 125))
	elif [ "$SLBV" -lt 149 ]; then
		SLBV=$(($SLBV - 123))
	elif [ "$SLBV" -lt 152 ]; then
		SLBV=$(($SLBV - 108))
	elif [ "$SLBV" -eq 152 ]; then
		SLBV="23"
	elif [ "$SLBV" -eq 153 ]; then
		SLBV="26"
	elif [ "$SLBV" -eq 154 ]; then
		SLBV="32"
	elif [ "$SLBV" -lt 158 ]; then
		SLBV=$(($SLBV - 30))
	elif [ "$SLBV" -lt 161 ]; then
		SLBV=$(($SLBV - 130))
	elif [ "$SLBV" -eq 161 ]; then
		SLBV="66"
	elif [ "$SLBV" -eq 162 ]; then
		SLBV="250"
	elif [ "$SLBV" -eq 163 ]; then
		SLBV="46"
	elif [ "$SLBV" -eq 166 ]; then
		SLBV="71"
	else
		SLBV="??"
	fi

}

idV=$(uci get modem.modem$CURRMODEM.idV)
idP=$(uci get modem.modem$CURRMODEM.idP)

if [ $idP = 1040 -o $idP = 1041 ]; then
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "telitinfo.gcom" "$CURRMODEM" | tr 'a-z' 'A-Z')
else
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "telitinfoln.gcom" "$CURRMODEM" | tr 'a-z' 'A-Z')
fi

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

TMP=$(echo $O" " | grep -o "#TEMPSENS: .\+ OK " | tr " " ",")
if [ -n "$TMP" ]; then
	TEMP=$(echo $TMP | cut -d, -f3)$(printf "\xc2\xb0")"C"
fi

MODE="-"
WS46=$(echo $O | grep -o "+COPS:.\+AT#RFSTS" | grep -o "+COPS: [0-3],[0-3],\"[^\"].\+\",[027]")
TECH=$(echo $WS46 | cut -d, -f4)
if [ -n "$TECH" ]; then
	MODE=$TECH
	case $MODE in
		"7")
			MODE="LTE"
			CAINFO=$(echo $OX | grep -o "#CAINFO: 1.\+OK")
			SGCELL=$(echo $OX | grep -o "[#^]RFSTS: \"[ 0-9]\{5,7\}\",[0-9]\{1,5\},.\+,\"[0-9]\{15\}\",\"[^\"]*\",[0-3],[0-9]\{1,2\}[,0-9]\{0,4\}")
			if [ -n "$SGCELL" ]; then
				RSCP=$(echo $SGCELL | cut -d, -f3)
				ECIO=$(echo $SGCELL | cut -d, -f5)
				RSSI=$(echo $SGCELL | cut -d, -f4)
				CSQ_RSSI=$(echo "$RSSI dBm")
				CHANNEL=$(echo $SGCELL | cut -d, -f2)
				if [ $(echo ${SGCELL:0:1}) = "#" ]; then
					LBAND="B"$(echo $SGCELL | cut -d, -f16 | grep -o "[0-9]\{1,2\}")
				else
					LBAND="B"$(echo $SGCELL | cut -d, -f15)
					SSINR=$(echo $SGCELL | cut -d, -f16)
					if [ -n "$SSINR" ]; then
						SINR=$((($(echo $SSINR) / 5) - 20))" dB"
					fi
				fi
				BW=$(echo $CAINFO | cut -d, -f3)
				decode_bw
				if [ -n "$BW" ];then
					LBAND=$LBAND" (Bandwidth $BW MHz)"
				fi
				if [ -n "$CAINFO" ]; then
					SCCLIST=$(echo $CAINFO | grep -o "1[2-6][0-9],[0-9]\{1,5\},[0-5],[0-9]\{1,3\},-[0-9]\+,-[0-9]\+,-[0-9]\+,[0-9]\{1,3\},2,[0-5],")
					if [ -n "$SCCLIST" ]; then
						SSINR=$(echo $CAINFO | grep -o "#CAINFO: [^,]\+,[^,]\+,[^,]\+,[^,]\+,[^,]\+,[^,]\+,[^,]\+,[0-9]\{1,3\},")
						SINR=$((($(echo $SSINR | cut -d, -f8) / 5) - 20))" dB"
						printf '%s\n' "$SCCLIST" | while read SCCVAL; do
							PCI=$(echo $SCCVAL | cut -d, -f4)
							SLBV=$(echo $SCCVAL | cut -d, -f1)
							decode_band
							LBAND=$LBAND"<br />B"$SLBV
							BW=$(echo $SCCVAL | cut -d, -f3)
							decode_bw
							LBAND=$LBAND" (CA, Bandwidth $BW MHz)"
							SCHV=$(echo $SCCVAL | cut -d, -f2)
							CHANNEL=$(echo "$CHANNEL", "$SCHV")
							{
								echo "$LBAND"
								echo "$CHANNEL"
							} > /tmp/lbandvar$CURRMODEM
						done
					fi
				fi
				if [ -e /tmp/lbandvar$CURRMODEM ]; then
					{
						read LBAND
						read CHANNEL
					} < /tmp/lbandvar$CURRMODEM
					rm /tmp/lbandvar$CURRMODEM
				fi
			fi
			;;
		2)
			MODE="UMTS"
			SGCELL=$(echo $O | grep -o "[#^]RFSTS: \"[ 0-9]\{5,7\}\",[0-9]\{1,5\},.\+,\"[0-9]\{15\}\",")
			if [ -n "$SGCELL" ]; then
				RSSI=$(echo $SGCELL | cut -d, -f6)
				CSQ_RSSI=$(echo "$RSSI dBm")
				RSCP=$(echo $SGCELL | cut -d, -f5)
				ECIO=$(echo $SGCELL| cut -d, -f4)
				CHANNEL=$(echo $SGCELL | cut -d, -f2)
			fi
			;;
	esac
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
}  > /tmp/signal$CURRMODEM.file

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
