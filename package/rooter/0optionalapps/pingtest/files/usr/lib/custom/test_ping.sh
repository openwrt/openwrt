#!/bin/sh

. /lib/functions.sh
 
ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
logger -t "Custom Ping Test " "$@"
}

sleep 20
CURRMODEM=1
uci set ping.ping.conn="1"
uci commit ping
while [ true ]
do
	INTER=$(uci get ping.ping.interval)
	ENB=$(uci get ping.ping.enable)
	if [ $ENB = 0 ]; then
		sleep $INTER
	else
		CONN=$(uci -q get modem.modem$CURRMODEM.connected)
		if [ $CONN = "1" ]; then
			result=`ps | grep -i "johns_ping.sh" | grep -v "grep" | wc -l`
			if [ $result -lt 1 ]; then
				/usr/lib/custom/johns_ping.sh &
			fi
		else
			uci set ping.ping.conn="1"
			uci commit ping
		fi
		sleep $INTER
	fi
done