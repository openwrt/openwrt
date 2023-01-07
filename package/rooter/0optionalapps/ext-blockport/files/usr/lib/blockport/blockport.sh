#!/bin/sh
. /lib/functions.sh

config="firewall"

do_block_remove() {
	config_get name $1 name
	if [ "$name" = "Block_src" ]; then
		uci delete $config".""$1" 
	fi
}

handle_port() {
	echo $1
	uci add $config rule
	uci set $config.@rule[-1].src='lan'
	uci set $config.@rule[-1].family='ipv4'
	uci set $config.@rule[-1].dest='wan'
	uci set $config.@rule[-1].target='DROP'
	uci set $config.@rule[-1].proto='tcp'
	uci set $config.@rule[-1].src_port="$1"
	uci set $config.@rule[-1].name='Block_src'
}
	
do_port() {
	config_list_foreach "$1" block handle_port
}

sleep 8
config_load $config
config_foreach do_block_remove rule

config_load blockport
config_foreach do_port port
uci commit $config
/etc/init.d/firewall restart 2>/dev/null

