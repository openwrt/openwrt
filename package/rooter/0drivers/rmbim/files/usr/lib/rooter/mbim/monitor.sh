#!/bin/sh

. /lib/functions.sh

ROOTER_LINK="/tmp/links"

log() {
	logger -t "MBIM Monitor" "$@"
}

CURRMODEM=$1
DEVICE=$2

while :; do
	tid=$(uci_get_state network wan$CURRMODEM tid)
	tid=$((tid + 1))
	umbim -d $DEVICE -n -t $tid status >/dev/null
	retq=$?
	uci_toggle_state network wan$CURRMODEM tid $tid
	[ $retq -ne 0 ] && break
	sleep 10
done

case $retq in
2)	 
	error="activation state: activating"
	;;
3)	 
	error="activation state: deactivated"
	;;
4)	 
	error="activation state: deactivating"
	;;
255)
	error="MBIM message not long enough"
	;;
esac

log "Modem $CURRMODEM Connection is Down ($error)"
if [ -f $ROOTER_LINK/reconnect$CURRMODEM ]; then
	$ROOTER_LINK/reconnect$CURRMODEM $CURRMODEM &
fi

# wait to be killed by mbim.sh
sleep 60
