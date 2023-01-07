#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	modlog "Delete SMS" "$@"
}

CURRMODEM=$1
COMMPORT="/dev/ttyUSB"$(uci get modem.modem$CURRMODEM.commport)
SMSLOC=$(uci -q get modem.modem$CURRMODEM.smsloc)

LOCKDIR="/tmp/smslock$CURRMODEM"
PIDFILE="${LOCKDIR}/PID"

while [ 1 -lt 6 ]; do
	if mkdir "${LOCKDIR}" &>/dev/null; then
		echo "$$" > "${PIDFILE}"
		ATCMDD="AT+CPMS=\"$SMSLOC\";+CMGD=1,4"
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		log "$OX"
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