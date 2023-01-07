#!/bin/sh

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

CURRMODEM=$1

if [ -e /etc/nosms ]; then
	uci set modem.modem$CURRMODEM.sms=0
	uci commit modem
	exit 0
fi

CPORT=$(uci get modem.modem$CURRMODEM.commport)
sleep 10
OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "smschk.gcom" "$CURRMODEM")

ERROR="ERROR"
if `echo ${OX} | grep "${ERROR}" 1>/dev/null 2>&1`
then
	uci set modem.modem$CURRMODEM.sms=0
	uci commit modem
else
	uci set modem.modem$CURRMODEM.sms=1
	uci commit modem
	if [ -e /usr/lib/sms/processsms ]; then
		if [ ! -e $ROOTER_LINK/processsms$CURRMODEM ]; then
			ln -s /usr/lib/sms/processsms $ROOTER_LINK/processsms$CURRMODEM
			$ROOTER_LINK/processsms$CURRMODEM $CURRMODEM &
		fi
	fi
fi
