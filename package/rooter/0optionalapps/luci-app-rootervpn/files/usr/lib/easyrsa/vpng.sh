#!/bin/sh
. /lib/functions.sh

convert() {
	secs=$1
	csec=$(printf "%02d:%02d:%02d" $(($secs/3600)) $(($secs%3600/60)) $(($secs%60)))
}

clist() {
	SDIR="/tmp/run"

	STAT=$SDIR"/openvpn."$1".status"

	COUNT=0
	RT=0
	TT="Virtual Address&#009;Common Name&#009;Real Address"
	while read -r line; do
		name="$line"
		RTT=$(echo "$name" | grep "Virtual Address")
		if [ ! -z "$RTT" ]; then
			RT=1
		fi
		RTT=$(echo "$name" | grep "GLOBAL STATS")
		if [ ! -z "$RTT" ]; then
			RT=0
		fi
		if [ $RT = 1 ]; then
			if [ $COUNT -gt 0 ]; then
				vip=$(echo "$name" | cut -d, -f1)
				com=$(echo "$name" | cut -d, -f2)
				rel=$(echo "$name" | cut -d, -f3)
				TT="$TT&#013;$vip&#009;$com&#009;$rel"
			fi
			let COUNT=COUNT+1
		fi
	done < "$STAT"
	if [ $COUNT -le 1 ]; then
		TT="No clients"
		COUNT=0
	else
		let COUNT=COUNT-1
	fi
}

rm -f /tmp/vpn

vpn_instance() {
	local s="$1"
	echo "$s   " >> /tmp/vpn
	config_get config "$s" config
	if [ -z $config ]; then
		config_get_bool client "$s" client 0
	else
		client=1
	fi
	echo "$client" >> /tmp/vpn
	config_get_bool enabled  "$s" 'enabled'  0
	echo "$enabled" >> /tmp/vpn

	result=`ps | grep -i "/usr/sbin/openvpn --syslog openvpn($s)" | grep -v "grep" | wc -l`
	if [ $result -ge 1 ]
   	then
		echo "1" >> /tmp/vpn
	else
		echo "0" >> /tmp/vpn
	fi
	
	if [ -z $config ]; then
		config_get port "$s" port
		if [ -z $port ]; then
			echo "1194" >> /tmp/vpn
		else
			echo "$port" >> /tmp/vpn
		fi
		config_get proto "$s" proto
		if [ -z $proto ]; then
			echo "udp" >> /tmp/vpn
		else
			echo "$proto" >> /tmp/vpn
		fi
	else
		prott=$(cat "$config" | grep -i "proto " | tr " " ",")
		prot=$(echo "$prott" | cut -d, -f2)
		remtt=$(cat "$config" | grep -i -m 1 "remote " | tr " " ",")
		remt=$(echo "$remtt" | cut -d, -f3)
		echo "$remt" >> /tmp/vpn
		echo "$prot" >> /tmp/vpn
	fi
	if [ $client = "0" ]; then
		echo "---" >> /tmp/vpn
		if [ $result -ge 1 ]; then
			clist $s
			echo "$COUNT" >> /tmp/vpn
			echo "$TT" >> /tmp/vpn
		else
			echo "0" >> /tmp/vpn
			echo "Not running" >> /tmp/vpn
		fi
	else
		if [ -z $config ]; then
			config_get remote "$s" remote
			echo "$remote" >> /tmp/vpn
		else
			remtt=$(cat "$config" | grep -i -m 1 "remote " | tr " " ",")
			remt=$(echo "$remtt" | cut -d, -f2)
			echo "$remt" >> /tmp/vpn
		fi
		echo "---" >> /tmp/vpn
		echo "not server" >> /tmp/vpn
	fi
	if [ $result -ge 1 ]
   	then
		config_get laststart "$s" laststart
		CURRTIME=$(date +%s)
		let upt=CURRTIME-laststart
		convert $upt
		upt=$csec
	else
		upt="---"
	fi
	echo "$upt" >> /tmp/vpn
}

config_load 'openvpn'
config_foreach vpn_instance 'openvpn'

exit 0

