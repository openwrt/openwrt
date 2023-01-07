#!/bin/sh

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
	logger -t "Reconnect Modem" "$@"
}

CURRMODEM=$1
uci set modem.modem$CURRMODEM.connected=0
uci commit modem

INTER=$(uci get modem.modeminfo$CURRMODEM.inter)

jkillall getsignal$CURRMODEM
rm -f $ROOTER_LINK/getsignal$CURRMODEM
jkillall con_monitor$CURRMODEM
rm -f $ROOTER_LINK/con_monitor$CURRMODEM
ifdown wan$INTER
MAN=$(uci get modem.modem$CURRMODEM.manuf)
MOD=$(uci get modem.modem$CURRMODEM.model)
$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Reconnecting"
PROT=$(uci get modem.modem$CURRMODEM.proto)

CPORT=$(uci get modem.modem$CURRMODEM.commport)

SEVR=$(uci get modem.modem$CURRMODEM.service)

if [ $SEVR = 0 ]; then
	COUNTER=1
	while [ $COUNTER -lt 6 ]; do
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "reset.gcom" "$CURRMODEM")
		ERROR="ERROR"
		if `echo ${OX} | grep "${ERROR}" 1>/dev/null 2>&1`
		then
			log "Retry Reset"
			sleep 3
  			let COUNTER=COUNTER+1
		else
			log "Modem Reset"
			sleep 3
			$ROOTER/common/lockchk.sh $CURRMODEM
			break
		fi
	done
	if [ $COUNTER -lt 6 ]; then
		ifup wan$INTER
	else
		log "Reset Failed for Modem $CURRMODEM"
		$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Failed to Reset"
	fi
else
	ifup wan$INTER
fi




