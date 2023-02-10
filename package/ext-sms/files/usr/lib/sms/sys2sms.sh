#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	logger -t "sms process" "$@"
}

ADDR="$1"
shift 1
TXT="$@"
MYPID=$(printf "%05d" $$)
RESFILE="/tmp/pdu"$MYPID

CURRMODEM=$(uci get modem.general.smsnum)
COMMPORT="/dev/ttyUSB"$(uci get modem.modem$CURRMODEM.commport)

RES=""
XSTATUS=0

lua /usr/lib/sms/sys2sms.lua "$ADDR" "$TXT" $MYPID

if [ -e $RESFILE ]; then
	read PDUL PDU < $RESFILE
	rm $RESFILE
else
	RES="Failed to write SMS - is text too long?"
	XSTATUS=1
	PDUL=""
	PDU=""
fi

LOCKDIR="/tmp/smslock$CURRMODEM"
PIDFILE="${LOCKDIR}/PID"

SMSLOC=$(uci -q get modem.modem$CURRMODEM.smsloc)
ATCMDD="$PDUL,$SMSLOC,0,$PDU"

while [ $XSTATUS -eq 0 ]; do
	if mkdir "${LOCKDIR}" &>/dev/null; then
		echo "$$" > "${PIDFILE}"
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "smswrite.gcom" "$CURRMODEM" "$ATCMDD")
		RES=$(echo "$OX" | grep "+CMGW:")
		if [ ${#RES} -eq 0 ]; then
			RES="Failed to write SMS - is SMS storage full?"
			XSTATUS=1
		else
			RES="New SMS written successfully"
		fi
		rm -rf "${LOCKDIR}"
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

log "$RES"
echo "$RES"
exit $XSTATUS
