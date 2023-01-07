#!/bin/sh

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
	logger -t "ROAM" "$@"
}

CURRMODEM=1
CPORT=$(uci get modem.modem$CURRMODEM.commport)
connect=$(uci get modem.modem$CURRMODEM.connected)
if [ "$connect" != "1" ]; then
	exit 0
fi

ATCMDD="AT+COPS=?"
#export TIMEOUT="120"
OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
#export TIMEOUT="5"
OX=$(echo "$OX " | sed -e "s!+COPS: !+COPS:,!g")
OX=$(echo "$OX" | grep "+COPS:")
if [ -z "$OX" ]; then
	rm -f /tmp/copseq
	exit 0
fi

CNT=2
rm -f /tmp/copseq
while [ true ]; do
	CCNT=$CNT
	AVAIL=$(echo $OX | cut -d, -f$CCNT)
	NXT=$(echo "$AVAIL" | grep "(")
	if [ ! -z "$NXT" ]; then
		AVAIL="${AVAIL#(}"
		let "CCNT=$CCNT+1"
		LONG=$(echo $OX | cut -d, -f$CCNT)
		temp="${LONG%\"}"
		LONG="${temp#\"}"
		let "CCNT=$CCNT+1"
		SHORT=$(echo $OX | cut -d, -f$CCNT)
		temp="${SHORT%\"}"
		SHORT="${temp#\"}"
		let "CCNT=$CCNT+1"
		NUMER=$(echo $OX | cut -d, -f$CCNT)
		temp="${NUMER%\"}"
		NUMER="${temp#\"}"
		let "CCNT=$CCNT+1"
		ACT=$(echo $OX | cut -d, -f$CCNT)
		ACT="${ACT%)}"
		echo "$AVAIL" >> /tmp/copseq
		echo "$LONG" >> /tmp/copseq
		echo "$SHORT" >> /tmp/copseq
		echo "$NUMER" >> /tmp/copseq
		echo "$ACT" >> /tmp/copseq
		let "CNT=$CCNT+1"
	else
		break
	fi
done
lang=$(uci -q get luci.main.lang)
/usr/lib/netroam/compress.lua $lang
