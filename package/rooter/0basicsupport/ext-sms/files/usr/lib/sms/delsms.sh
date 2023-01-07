#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	modlog "Delete SMS" "$@"
}

CURRMODEM=$1
shift 1
SLOTS="$@"
log "$SLOTS"

COMMPORT="/dev/ttyUSB"$(uci get modem.modem$CURRMODEM.commport)

SMSLOC=$(uci -q get modem.modem$CURRMODEM.smsloc)

LOCKDIR="/tmp/smslock$CURRMODEM"
PIDFILE="${LOCKDIR}/PID"

while [ true ]; do
	if mkdir "${LOCKDIR}" &>/dev/null; then
		echo "$$" > "${PIDFILE}"
		for SLOT in $SLOTS
		do
			ATCMDD="AT+CPMS=\"$SMSLOC\";+CMGD=$SLOT"
			OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
			log "$OX"
		done
		uci set modem.modem$CURRMODEM.smsnum=999
		uci commit modem
		break
	else
		OTHERPID="$(cat "${PIDFILE}")"
		if [ $? = 0 ]; then
			if ! kill -0 $OTHERPID &>/dev/null; then
				rm -rf "${LOCKDIR}"
			fi
		fi
		sleep 1
	fi
done
rm -rf "${LOCKDIR}"
