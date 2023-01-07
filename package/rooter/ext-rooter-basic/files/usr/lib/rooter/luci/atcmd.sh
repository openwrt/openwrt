#!/bin/sh

ROOTER=/usr/lib/rooter
ATCMDD=$1

CURRMODEM=$(uci get modem.general.miscnum)
COMMPORT="/dev/ttyUSB"$(uci get modem.modem$CURRMODEM.commport)

M2=$(echo "$ATCMDD" | sed -e "s#~#\"#g")
COPS="+cops=?"
QOPS="+qops?"
M3=$(echo "$M2" | awk '{print tolower($0)}')
if `echo ${M3} | grep "${COPS}" 1>/dev/null 2>&1`; then
	export TIMEOUT="120"
elif `echo ${M3} | grep "${QOPS}" 1>/dev/null 2>&1`; then
	export TIMEOUT="120"
else
	export TIMEOUT="5"
fi
OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$M2")
echo "$OX" > /tmp/result$CURRMODEM.at
