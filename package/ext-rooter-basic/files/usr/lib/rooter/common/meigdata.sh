#!/bin/sh 

ROOTER=/usr/lib/rooter

log() {
	logger -t "Meig Data" "$@"
}

CURRMODEM=$1
COMMPORT=$2

OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "meiginfo.gcom" "$CURRMODEM" | tr 'a-z' 'A-Z')

O=$($ROOTER/common/processat.sh "$OX")
O=$(echo $O)
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

MODE="-"
PSRAT=$(echo $O" " | grep -o "+PSRAT: .\+ OK " | tr " " ",")
TECH=$(echo $PSRAT | cut -d, -f2)
if [ -n "$TECH" ]; then
	MODE=$TECH
fi

ATECH=$(echo $OX | grep -o "+SGCELLINFOEX:[^,]\{2,6\}," | grep -o ":[ ]\?[-A-Z5]\{2,6\}" | grep -o "[-A-Z5]\{2,6\}")
if [ -n "$ATECH" ]; then
	MODE=$ATECH
fi

SGCELL=$(echo $O" " | grep -o "+SGCELLINFO: .\+ OK " | tr " " ",")

MODEx=$MODE
case $MODEx in
	"TD-LTE"|"FDD-LTE"|"FDD" )
		RSSI=$(echo $SGCELL | cut -d, -f5)
		CSQ_RSSI="-"$(echo $RSSI | grep -o "[0-9]\{1,3\}")" dBm"
		RSCP=$(echo $SGCELL | cut -d, -f6)
		RSCP="-"$(echo $RSCP | grep -o "[0-9]\{1,3\}")
		ECIO=$(echo $SGCELL| cut -d, -f7)
		ECIO="-"$(echo $ECIO | grep -o "[0-9]\{1,3\}")
		SSINR=$(echo $SGCELL | cut -d, -f8 | grep -o "[0-9]\{1,3\}")
		if [ -n "$SSINR" -a "$SSINR" -le "250" ]; then
			SINR=$((($SSINR / 5) - 20))" dB"
		fi
		LBAND=$(echo $SGCELL | cut -d, -f9)
		LBAND=$(echo $LBAND | grep -o "[0-9]\{1,5\}")
		let LBAND=LBAND-119
		LBAND="B"$LBAND
		CHANNEL=$(echo $SGCELL | cut -d, -f10)
		CHANNEL=$(echo $CHANNEL | grep -o "[0-9]\{1,5\}")
		ICELL=$(echo $O" " | grep -o "+CELLINFO: .\+ OK " | tr " " ",")
		PCI=$(echo $ICELL | cut -d, -f18)
		PCI=$(echo $PCI | grep -o "[0-9]\{1,5\}")
		if [ $MODE = "FDD" ]; then
			MODE="FDD LTE"
		fi
		;;
	"LTE"|"EN-DC" )
		SGC=$(echo $OX | grep -o "+SGCELLINFOEX:[ ]\?[-CDELNTE]\{3,5\},.\+AT+CELLINFO")
		MODE=$(echo $SGC | cut -d, -f2)
		PCI=$(echo $SGC | cut -d, -f6)
		LBAND="B"$(echo $SGC | cut -d, -f10)
		BW=$(echo $SGC | cut -d, -f11 | grep -o "[0-9]\{1,3\}")
		if [ $BW -gt 14 ]; then
			let BW=BW/5
		else
			BW="1.4"
		fi
		LBAND=$LBAND" (Bandwidth $BW Mhz)"
		CHANNEL=$(echo $SGC | cut -d, -f12)
		RSSI=$(echo $SGC | cut -d, -f14)
		CSQ_RSSI=$(echo $RSSI | grep -o "[-0-9]\{1,3\}")" dBm"
		RSCP=$(echo $SGC | cut -d, -f15)
		ECIO=$(echo $SGC| cut -d, -f16)
		SINR=$(echo $SGC | cut -d, -f18)" dB"
		if [ $MODEx == "EN-DC" ]; then
			NRSINR=$(echo $SGC | cut -d, -f31 | grep -o "[-0-9]\{1,4\}")
			NRBAND=$(echo $SGC | cut -d, -f32 | grep -o "[0-9]\{1,3\}")
			NRCHAN=$(echo $SGC | cut -d, -f33 | grep -o "[0-9]\{6,7\}")
			NRBW=$(echo $SGC | cut -d, -f34 | grep -o "[0-9]\{1,3\}")
			if [ -n "$NRBAND" -a -n "$NRCHAN" -a -n "$NRBW" -a -n $NRSINR ]; then
				NRRSRP=$(echo $SGC | cut -d, -f29 | grep -o "[-0-9]\{2,4\}")
				NRRSRQ=$(echo $SGC | cut -d, -f30 | grep -o "[-0-9]\{2,4\}")
				NRPCI=$(echo $SGC | cut -d, -f35 | grep -o "[0-9]\{1,3\}")
				let NRBW=NRBW/5
				LBAND=$LBAND"<br />n"$NRBAND" (Bandwidth "$NRBW" MHz)"
				RSCP=$RSCP" dBm<br />"$NRRSRP
				ECIO=$ECIO" dBm<br />"$NRRSRQ
				SINR=$SINR"<br />"$((($NRSINR / 5) - 20))" dB"
				CHANNEL=$CHANNEL","$NRCHAN
				PCI=$PCI","$NRPCI
			fi
		fi
		;;
	"5G" )
		SGC=$(echo $OX | grep -o "+SGCELLINFOEX:[ ]\?5G,.\+AT+CELLINFO")
		MODE=$(echo $SGC | cut -d, -f2)
		PCI=$(echo $SGC | cut -d, -f6)
		LBAND="n"$(echo $SGC | cut -d, -f8)
		BW=$(echo $SGC | cut -d, -f9 | grep -o "[0-9]\{1,3\}")
		let BW=BW/5
		LBAND=$LBAND" (Bandwidth $BW Mhz)"
		CHANNEL=$(echo $SGC | cut -d, -f12)
		RSCP=$(echo $SGC | cut -d, -f15)
		ECIO=$(echo $SGC| cut -d, -f16)
		SINR=$(echo $SGC | cut -d, -f17)
		SINR=$((($SINR / 5) - 20))" dB"
		;;
	"HSPA+"|"HSUPA"|"HSDPA"|"WCDMA" )
		if [ -n "$ATECH" ]; then
			SGC=$(echo $OX | grep -o "WCDMA,[0-9]\{3\},.\+AT+CELLINFO" | tr ' ' ',')
			CHANNEL=$(echo $SGC | cut -d, -f11)
			ECIO=$(echo $SGC | cut -d, -f14)
			RSCP=$(echo $SGC | cut -d, -f16)
		else
			RSCP=$(echo $SGCELL | cut -d, -f11)
			RSCP="-"$(echo $RSCP | grep -o "[0-9]\{1,3\}")
			ECIO=$(echo $SGCELL| cut -d, -f12)
			ECIO="-"$(echo $ECIO | grep -o "[0-9]\{1,3\}")
			CHANNEL=$(echo $SGCELL | cut -d, -f8)
			CHANNEL=$(echo $CHANNEL | grep -o "[0-9]\{1,4\}")
			RSSI=$(echo $SGCELL | cut -d, -f10)
			CSQ_RSSI=$(echo $RSSI | grep -o "[0-9]\{1,3\}")" dBm"
		fi
		;;
	*)
		RSCP=$(echo $SGCELL | cut -d, -f10)
		RSCP="-"$(echo $RSCP | grep -o "[0-9]\{1,3\}")
		CHANNEL=$(echo $SGCELL | cut -d, -f8)
		CHANNEL=$(echo $CHANNEL | grep -o "[0-9]\{1,4\}")
		RSSI=$(echo $SGCELL | cut -d, -f9)
		CSQ_RSSI=$(echo $RSSI | grep -o "[0-9]\{1,3\}")" dBm"
		;;
esac

CALIST=$(echo "${OX//[ ]/}" | grep -o "+CELLINFO:\"SCC\",[0-9],2,[0-9],[0-9]\{1,6\},[0-9]\{1,4\},[0-9]\{1,3\},[0-9]\{1,3\},[-0-9]\{1,4\},[-0-9]\{1,4\},[-0-9]\{1,4\},[-0-9]\{1,4\}")

for CAVAL in $(echo "$CALIST"); do
	if [ "$(echo $CAVAL | cut -d, -f2)" == "1" ]; then
		CATYPE="CA"$(printf "\xe2\x86\x91")
	else
		CATYPE="CA"
	fi
	CHANv=$(echo $CAVAL | cut -d, -f5)
	BWv=$(echo $CAVAL | cut -d, -f6)
	if [ $BWv -gt 14 ]; then
		let BWv=BWv/5
	else
		BWv="1.4"
	fi
	if [ $CHANv -ge 123400 ]; then
		BANDv="n"$(echo $CAVAL | cut -d, -f7)
	else
		BANDv="B"$(echo $CAVAL | cut -d, -f7)
	fi
	LBAND=$LBAND"<br />"$BANDv" ($CATYPE, Bandwidth "$BWv" MHz)"
	PCI=$PCI","$(echo $CAVAL | cut -d, -f8)
	ECIO=$ECIO" dBm<br />"$(echo $CAVAL | cut -d, -f9)
	RSCP=$RSCP" dBm<br />"$(echo $CAVAL | cut -d, -f10)
	SINRv=$(echo $CAVAL | cut -d, -f12)
	SINRv=$((($SINRv / 5) - 20))" dB"
	CHANNEL=$CHANNEL","$CHANv
	SINR=$SINR"<br />"$SINRv
done

NETMODE="-"
NMODE=$(echo $O" " | grep -o "+MODODR: .\+ OK " | tr " " ",")
TECH=$(echo $NMODE | cut -d, -f2)
if [ -n "$TECH" ]; then
	MODTYPE="7"
	case $TECH in
	"2" )
		NETMODE="1" # Auto
		;;
	"1" )
		NETMODE="5" # 3G only
		;;
	"4" )
		NETMODE="4" # 3G preferred
		;;
	"3" )
		NETMODE="3" # 2G only
		;;
	"5" )
		NETMODE="7" # LTE only
		;;
	* )
		NETMODE="1"
		;;
	esac
fi
RATs=$(echo "$OX" | grep -o "\^SYSCFGEX: \"[0-9]\{2,6\}\"" | grep -o "[0-9]\{2,6\}")
if [ -n "$RATs" ]; then
	MODTYPE="7"
	case $RATs in
	"02" )
		NETMODE="5" ;;
	"03" )
		NETMODE="7" ;;
	"04" )
		NETMODE="9" ;;
	"0203" | "0204" | "020304" | "020403" )
		NETMODE="4" ;;
	"0304" | "0302" | "030402" | "030204" )
		NETMODE="6" ;;
	"0403" )
		NETMODE="8" ;;
	* )
		NETMODE="1" ;;
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
}  > /tmp/signal$CURRMODEM.file

CONNECT=$(uci get modem.modem$CURRMODEM.connected)
if [ "$CONNECT" == "0" ]; then
    exit 0
fi

if [ "$CSQ" = "-" ]; then
	log "$OX"
fi

WWANX=$(uci get modem.modem$CURRMODEM.interface)
OPER=$(cat /sys/class/net/$WWANX/operstate 2>/dev/null)
rm -f "/tmp/connstat"$CURRMODEM

if [ ! "$OPER" ]; then
	exit 0
fi
if echo $OPER | grep -q "unknown"; then
	exit 0
fi

if echo $OPER | grep -q "down"; then
	echo "1" > "/tmp/connstat"$CURRMODEM
fi
