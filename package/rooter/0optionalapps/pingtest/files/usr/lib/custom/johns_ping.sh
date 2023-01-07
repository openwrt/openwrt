#!/bin/sh

. /lib/functions.sh
 
ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
logger -t "Custom Ping Test " "$@"
}

tping() {
	hp=$(httping $2 -c 3 -s $1)
	pingg=$(echo $hp" " | grep -o "round-trip .\+ ms ")
	if [ -z "$pingg" ]; then
		tmp=0
	else
		tmp=200
	fi
}

doping() {
	TYPE=$(uci get ping.ping.type)
	if [ $TYPE = "1" ]; then
	log "Curl"
		RETURN_CODE_1=$(curl -m 10 -s -o /dev/null -w "%{http_code}" $ipv41)
		RETURN_CODE_2=$(curl --ipv6 -m 10 -s -o /dev/null -w "%{http_code}" $ipv6)
		RETURN_CODE_3=$(curl -m 10 -s -o /dev/null -w "%{http_code}" $ipv42)
	else
	log "Ping"
		tping "$ipv41"; RETURN_CODE_1=$tmp
		tping "$ipv6" "-6"; RETURN_CODE_2=$tmp
		tping "$ipv42"; RETURN_CODE_3=$tmp
	fi
}

ptest() {
	tries=0
	status=0
	while [ $tries -lt $1 ]
	do
		CONN=$(uci -q get modem.modem$CURRMODEM.connected)
		if [ $CONN = "1" ]; then
			uci set ping.ping.conn="4"
			uci commit ping
			doping
			if [[ "$RETURN_CODE_1" != "200" &&  "$RETURN_CODE_2" != "200" &&  "$RETURN_CODE_3" != "200" ]]; then
				uci set ping.ping.conn="1"
				uci commit ping
				status=1
				return
			fi
			log "Second Ping Test Good"
			uci set ping.ping.conn="2"
			uci commit ping
			status=0
			return
		else
			sleep 20
			tries=$((tries+1))
		fi
	done
	status=1
}

ipv41=$(uci -q get ping.ping.ipv41)
ipv42=$(uci -q get ping.ping.ipv42)
ipv6=$(uci -q get ping.ping.ipv6)
uci set ping.ping.conn="4"
uci commit ping
	
CURRMODEM=1
CPORT=$(uci -q get modem.modem$CURRMODEM.commport)
DELAY=$(uci get ping.ping.delay)

doping

if [[ "$RETURN_CODE_1" != "200" &&  "$RETURN_CODE_2" != "200" &&  "$RETURN_CODE_3" != "200" ]]; then
	log "Bad Ping Test"
	doping
	if [[ "$RETURN_CODE_1" != "200" &&  "$RETURN_CODE_2" != "200" &&  "$RETURN_CODE_3" != "200" ]]; then
		log "Second Bad Ping Test"
		uci set ping.ping.conn="3"
		uci commit ping
		log "Restart Network"
		/usr/lib/rooter/luci/restart.sh $CURRMODEM 10
		sleep $DELAY
		ptest 3
		if [ $status -eq 0 ]; then
			log "Good Ping after Network Restart"
			uci set ping.ping.conn="2"
			uci commit ping
			exit 0
		else
			log "Hard Restart"
			/usr/lib/rooter/luci/restart.sh $CURRMODEM 11
			ptest 9
			if [ $status -eq 0 ]; then
				log "Good Ping after Hard Restart"
				uci set ping.ping.conn="2"
				uci commit ping
				exit 0
			else
				reboot -f
			fi
		fi
	fi
else
	log "Good Ping"
	uci set ping.ping.conn="2"
	uci commit ping
fi
exit 0
