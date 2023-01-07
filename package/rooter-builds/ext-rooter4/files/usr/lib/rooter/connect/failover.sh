#!/bin/sh
. /lib/functions.sh

ROOTER=/usr/lib/rooter

log() {
	logger -t "Failover System" "$@"
}

log "Failover System is Started"

ifname1="ifname"
if [ -e /etc/newstyle ]; then
	ifname1="device"
fi

i=1
STAT="0"
track_ips=
WAN_STATUS="0"
MODEM_STATUS="0"
MODEM_IFUP=true
rm -f /tmp/wanstatus
rm -f /tmp/modemstatus
rm -f /tmp/mdown$CURRMODEM

get_interface() {
	OX=$1
	case $OX in
	"Wan" )
		inter="wan"
		;;
	"Hotspot" )
		inter="wwan"
		;;
	* )
		inter="wan${OX:5}"
		;;
	esac
	uci set failover.$OX.interface=$inter
}

ping_interface() {
	interf=$(uci get failover.$1.interface)
	if [ $interf = "wwan" ]; then
		IFN="$(ubus -S call network.wireless status | jsonfilter -e '@.*.interfaces[@.config.mode="sta"].${ifname1}')"
	else
		IFN=$(uci get network.$interf.${ifname1})
	fi
	if [ ! -z $IFN ]; then
		STAT="1"
		host_up_count=0
		score_up=$UP
		score_dwn=$DOWN
		lost=0
		while true; do
			if [ ! -z "$track_ips" ]; then
				for track_ip in $track_ips; do
					ping -I $IFN -c $COUNT -W $TIMEOUT -s 4 -q $track_ip &> /dev/null
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
						STAT="1"
						break
					fi
				else
					let score_up--
					score_dwn=$DOWN
					if [ $score_up -eq 0 ]; then
						STAT="2"
						break
					fi
				fi
			else
				log "No Tracking IP, stopping Failover"
				exit
			fi
			host_up_count=0
			sleep $INTERVAL
		done
	else
		STAT="0"
	fi
}

make_status() {
	name=$2
	status=$3
	interface=$(uci get failover.$name.interface)
	echo "$name" > /tmp/$1"status"
	echo "$interface" >> /tmp/$1"status"
	case $status in
	"0" )
		echo "disabled" >> /tmp/$1"status"
		;;
	"1" )
		echo "offline" >> /tmp/$1"status"
		;;
	"2" )
		echo "online" >> /tmp/$1"status"
		;;
	"3" )
		echo "onlinedwn" >> /tmp/$1"status"
		;;
	esac
}

list_track_ips() {
	track_ips="$1 $track_ips"
}

rm -f /tmp/wanstatus
rm -f /tmp/modemstatus

config_load failover
config_list_foreach "failover" "trackip" list_track_ips

use_wan=$(uci get failover.faillist.use_wan)
get_interface $use_wan
use_modem=$(uci get failover.faillist.use_modem)
get_interface $use_modem
uci commit failover

interf=$(uci get failover.$use_wan.interface)
uci set network.$interf.metric="99"
uci commit network
ifup $interf

TIMEOUT=$(uci get failover.failover.pingwait)
INTERVAL=$(uci get failover.failover.pingtime)
RELIAB=$(uci get failover.failover.reliability)
DOWN=$(uci get failover.failover.down)
UP=$(uci get failover.failover.up)
COUNT=$(uci get failover.failover.count)

while true; do
	use_wan=$(uci get failover.faillist.use_wan)
	use_modem=$(uci get failover.faillist.use_modem)
	ping_interface $use_wan
	make_status "wan" $use_wan $STAT
	WAN_STATUS=$STAT
	if [ $WAN_STATUS = "2" ];then
		if [ $MODEM_IFUP = true -a $MODEM_STATUS == "2" ]; then
			MODEM_IFUP=false
			MODEM_STATUS="3"
			ifdown $(uci get failover.$use_modem.interface)
		fi
	else
		if [ $MODEM_IFUP = false  -a $MODEM_STATUS != "0" ]; then
			MODEM_IFUP=true
			MODEM_STATUS="2"
			ifup $(uci get failover.$use_modem.interface)
			sleep 10
		fi
	fi

	if [ $MODEM_STATUS != "3" ]; then
		ping_interface $use_modem
		make_status "modem" $use_modem $STAT
		MODEM_STATUS=$STAT
		MODEM_IFUP=true
		if [ $STAT = "1" ]; then
			echo "0" > /tmp/mdown$CURRMODEM
		else
			rm -f /tmp/mdown$CURRMODEM
		fi
	else
		interf=$(uci get failover.$use_modem.interface)
		IFN=$(uci get network.$interf.${ifname1})
		if [ -z $IFN ]; then
			MODEM_IFUP=false
			make_status "modem" $use_modem "0"
			MODEM_STATUS="0"
		else
			make_status "modem" $use_modem "3"
			MODEM_STATUS="3"
			MODEM_IFUP=false
		fi
	fi
	sleep 10
done
