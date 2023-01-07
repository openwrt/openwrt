#!/bin/sh

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
	logger -t "ROAM" "$@"
}

mccmnc=$1

log "Disconnect modem"

CURRMODEM=1
uci set modem.modem$CURRMODEM.connected=0
uci commit modem
INTER=$(uci get modem.modeminfo$CURRMODEM.inter)

jkillall getsignal$CURRMODEM
rm -f $ROOTER_LINK/getsignal$CURRMODEM
jkillall con_monitor$CURRMODEM
rm -f $ROOTER_LINK/con_monitor$CURRMODEM
ifdown wan$INTER

PROT=$(uci get modem.modem$CURRMODEM.proto)
CPORT=$(uci get modem.modem$CURRMODEM.commport)

case $PROT in
"30" )
	ATCMDD="AT+CFUN=0"
	$ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD"
	;;
"3" )
	WDMNX=$(uci get modem.modem$CURRMODEM.wdm)
	umbim -n -t 3 -d /dev/cdc-wdm$WDMNX disconnect
	;;
* )
	$ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "reset.gcom" "$CURRMODEM"
	;;
esac

log "Lock to network"

mcc=${mccmnc:0:3}
mnc=${mccmnc:3}

ATCMDD="AT+COPS=1,2,\"$mccmnc\",7"
echo "ATCMDD=\"$ATCMDD\"" > /tmp/rlock

log "Reconnect Modem"

$ROOTER_LINK/create_proto$CURRMODEM $CURRMODEM 1 &

