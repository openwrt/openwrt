#!/bin/sh

TYPE=$1
DIREC=$2
source /tmp/modcnt
MODCNT=$MODCNTX

case $TYPE in
"modem" )
	MODENUM=$(uci get modem.general.modemnum)
	;;
"sms" )
	MODENUM=$(uci get modem.general.smsnum)
	;;
"misc" )
	MODENUM=$(uci get modem.general.miscnum)
	;;
esac

if [ $DIREC -eq 1 ]; then
	let MODENUM=MODENUM+1
	if [ $MODENUM -gt $MODCNT ]; then
		MODENUM=1
	fi
else
	if [ $MODENUM -eq 1 ]; then
		MODENUM=$MODCNT
	else
		let MODENUM=MODENUM-1
	fi
fi
case $TYPE in
"modem" )
	uci set modem.general.modemnum=$MODENUM
	;;
"sms" )
	uci set modem.general.smsnum=$MODENUM
	;;
"misc" )
	uci set modem.general.miscnum=$MODENUM
	uci commit modem
	/usr/lib/rooter/luci/mask.sh
	;;
esac
uci commit modem

