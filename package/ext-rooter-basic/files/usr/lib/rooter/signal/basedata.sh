#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	logger -t "basedata" "$@"
}

CURRMODEM=$1
COMMPORT=$2

get_base() {
	echo "0" > /tmp/block
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "baseinfo.gcom" "$CURRMODEM")
	O=$($ROOTER/common/processat.sh "$OX")
	rm -f /tmp/block
}

get_base

DOWN=$(echo "$O" | awk -F[,] '/\+CGEQNEG:/ {printf "%s", $4}')
if [ "x$DOWN" != "x" ]; then
	UP=$(echo "$O" | awk -F[,] '/\+CGEQNEG:/ {printf "%s", $3}')
	DOWN=$DOWN" kbps Down | "
	UP=$UP" kbps Up"
else
	DOWN="-"
	UP="-"
fi

MANUF=$(echo "$O" | awk -F[:] '/Manufacturer:/ { print $2}')
if [ "x$MANUF" = "x" ]; then
	MANUF=$(uci get modem.modem$CURRMODEM.manuf)
fi

MODEL=$(echo "$O" | awk -F[,\ ] '/^\+MODEL:/ {print $2}')
if [ "x$MODEL" != "x" ]; then
	MODEL=$(echo "$MODEL" | sed -e 's/"//g')
	if [ $MODEL = 0 ]; then
		MODEL = "mf820"
	fi
else
	MODEL=$(uci get modem.modem$CURRMODEM.model)
fi
MODEM=$MANUF" "$MODEL

pval=$(uci get modem.modem$CURRMODEM.proto)
case $pval in
"1" )
	PROTO="Direct-IP"
	;;
"2" )
	PROTO="QMI"
	;;
"3"|"30" )
	PROTO="MBIM"
	;;
"6"|"4"|"7"|"24"|"26"|"27"|"28" )
	PROTO="NCM"
	;;
"10"|"11"|"12"|"13"|"14"|"15" )
	PROTO="PPP"
	;;
"5" )
	PROTO="Ethernet"
	;;
"9" )
	PROTO="ipheth"
	;;
esac

echo 'MODEM="'"$MODEM"'"' >> /tmp/base$CURRMODEM.file
echo 'DOWN="'"$DOWN"'"' >> /tmp/base$CURRMODEM.file
echo 'UP="'"$UP"'"' >> /tmp/base$CURRMODEM.file
echo 'PROTO="'"$PROTO"'"' >> /tmp/base$CURRMODEM.file
