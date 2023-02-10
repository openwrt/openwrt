#!/bin/sh
. /lib/functions.sh

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
	modlog "Connection Monitor $CURRMODEM" "$@"
}


pingtest(){
	UPDWN="0"
	host_up_count=0
	score_up=$UP
	score_dwn=$DOWN
	lost=0
	while true; do
		if [ ! -z "$track_ips" ]; then
			for track_ip in $track_ips; do
				ping -I $IFNAME -c $COUNT -W $TIMEOUT -s $PACKETSIZE -q $track_ip &> /dev/null
				if [ $? -eq 0 ]; then
					let host_up_count++
				else
					let lost++
				fi
			done
			if [ $host_up_count -lt $RELIAB ]; then
				let score_dwn--
				score_up=$UP
				if [ $score_dwn -eq 0 ]; then
					UPDWN="1"
					break
				fi
			else
				let score_up--
				score_dwn=$DOWN
				if [ $score_up -eq 0 ]; then
					UPDWN="0"
					break
				fi
			fi
		else
			UPDWN="0"
			log "Missing Tracking IP"
			exit
		fi
		host_up_count=0
		sleep $INTERVAL
	done
	if [ $UPDWN = "1" ]; then # failed ping test
		RESTART=1
	else
		echo 'MONSTAT="'"UP ($CURSOR) (using Ping Test)"'"' > /tmp/monstat$CURRMODEM
		RESTART=0
	fi
}

restart() {
	if [ $ACTIVE = "2" ]; then 
		reboot -f
	else
		/usr/lib/rooter/luci/restart.sh $CURRMODEM 11
		exit 0
	fi
}

CURRMODEM=$1
CURSOR="-"
ACTIVE=$(uci get modem.pinginfo$CURRMODEM.alive)
if [ $ACTIVE = "0" ]; then
	echo 'MONSTAT="'"Disabled"'"' > /tmp/monstat$CURRMODEM
	exit
fi

track_ips=
IFNAME=$(uci get modem.modem$CURRMODEM.interface)
TIMEOUT=$(uci get modem.pinginfo$CURRMODEM.pingwait)
INTERVAL=$(uci get modem.pinginfo$CURRMODEM.pingtime)
RELIAB=$(uci get modem.pinginfo$CURRMODEM.reliability)
DOWN=$(uci get modem.pinginfo$CURRMODEM.down)
UP=$(uci get modem.pinginfo$CURRMODEM.up)
COUNT=$(uci get modem.pinginfo$CURRMODEM.count)
PACKETSIZE=$(uci get modem.pinginfo$CURRMODEM.packetsize)
INTERF=$(uci get modem.modeminfo$CURRMODEM.inter)

list_track_ips() {
	track_ips="$1 $track_ips"
}

config_load modem
config_list_foreach "pinginfo$CURRMODEM" "trackip" list_track_ips

while [ true ]; do
	CP=$(uci -q get ping.ping.enable)
	if [ $CP = "1" ]; then
		echo 'MONSTAT="'"Custom Ping Test"'"' > /tmp/monstat$CURRMODEM
		sleep 60
	else
		# check to see if modem iface has an IP address, if not try a reconnect
		OX=$(ip address show $IFNAME 2>&1)
		ip4=$(echo "$OX" | grep 'inet ' | cut -d' ' -f6)
		ip6=$(echo "$OX" | grep 'inet6' | grep global | cut -d' ' -f6)
		if [ -z "$ip4" -a -z "$ip6" ]; then
log "No IP Address, Exiting"
			echo 'MONSTAT="'"DOWN (no IP address)"'"' > /tmp/monstat$CURRMODEM
			/usr/lib/rooter/luci/restart.sh $CURRMODEM 11
			exit 0
		fi
		pingtest
		if [ $RESTART = "1" ]; then  # fail first ping test
log "Bad Ping Test, Restart"
			echo 'MONSTAT="'"DOWN Bad Ping)"'"' > /tmp/monstat$CURRMODEM
			restart
		fi
		if [ $CURSOR = "-" ]; then
			CURSOR="+"
		else
			CURSOR="-"
		fi
		sleep $INTERVAL
	fi
done