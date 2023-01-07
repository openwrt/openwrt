#!/bin/sh

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"
TIMEOUT=10

log() {
	modlog "Disconnect Modem $CURRMODEM" "$@"
}

handle_timeout(){
	local wget_pid="$1"
	local count=0
	res=1
	if [ -d /proc/${wget_pid} ]; then
		res=0
	fi
	while [ "$res" = 0 -a $count -lt "$((TIMEOUT))" ]; do
		sleep 1
		count=$((count+1))
		res=1
		if [ -d /proc/${wget_pid} ]; then
			res=0
		fi
	done

	if [ "$res" = 0 ]; then
		log "Killing process on timeout"
		kill "$wget_pid" 2> /dev/null
		res=1
		if [ -d /proc/${wget_pid} ]; then
			res=0
		fi
		if [ "$res" = 0 ]; then
			log "Killing process on timeout"
			kill -9 $wget_pid 2> /dev/null	
		fi
	fi
}

CURRMODEM=$(uci get modem.general.miscnum)
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
$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Disconnected"

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

$ROOTER/log/logger "Modem #$CURRMODEM was Manually Disconnected"
