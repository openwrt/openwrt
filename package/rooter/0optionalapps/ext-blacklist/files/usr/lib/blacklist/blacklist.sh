#!/bin/sh 
. /lib/functions.sh

log() {
	logger -t "Blacklist" "$@"
}

getmac() {
	local config=$1
	config_get src_mac $1 src_mac
	if [ ! -z $src_mac ]; then
		if [ $create = "0" ]; then
			uci set firewall.blacklist=rule
			uci set firewall.blacklist.src='lan'
			uci set firewall.blacklist.target='REJECT'
			uci set firewall.blacklist.dest='wan'
			uci set firewall.blacklist.name='Blacklist'		
			create=1
		fi
		uci add_list firewall.blacklist.src_mac=$src_mac
	fi
}
	
sleep 8
create="0"
uci -q delete firewall.blacklist
config_load blacklist
config_foreach getmac devices
uci commit firewall
/etc/init.d/firewall restart

exit 0
