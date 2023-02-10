#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	modlog "SimLock $CURRMODEM" "$@"
}

CURRMODEM=$1
CPORT=$(uci get modem.modem$CURRMODEM.commport)

ATCMDD="at+cpin?"
OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
ERR=$(echo "$OX" | grep "ERROR")
if [ ! -z "$ERR" ]; then # No SIM
	log "No SIM"
	echo "2" > /tmp/simpin$CURRMODEM
	exit 0
fi
RDY=$(echo "$OX" | grep "READY")
if [ -z "$RDY" ]; then # SIM Locked
	spin=$(uci -q get custom.simpin.pin) # SIM Pin
	if [ -z "$spin" ]; then
		spin=$(uci -q get modem.modeminfo$CURRMODEM.pincode) # Profile Pin
		if [ -z "$spin" ]; then
			spin=$(uci -q get profile.simpin.pin) # Default profile Pin
			if [ -z "$spin" ]; then
				echo "0" > /tmp/simpin$CURRMODEM # Locked/No Pin
				log "Locked/No Pin"
				exit 0
			fi
		fi
	fi
	export PINCODE="$spin" # Use Pin to unlock
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "setpin.gcom" "$CURRMODEM")
	sleep 5
	ATCMDD="at+cpin?"
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	RDY=$(echo "$OX" | grep "READY")
	if [ -z "$RDY" ]; then # sim locked
		echo "1" > /tmp/simpin$CURRMODEM # Incorrect Pin
		log "Incorrect Pin"
		exit 0
	fi
	log "Correct Pin"
	$ROOTER/common/gettype.sh $CURRMODEM
else
	log "Not Locked"
	sblk=$(uci -q get custom.simpin.block)
	if [ "$sblk" = "1" ]; then
		echo "4" > /tmp/simpin$CURRMODEM
		log "Unlocked not allowed"
		exit 0
	fi
fi
rm -f /tmp/simpin$CURRMODEM # not locked

