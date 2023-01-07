#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "Wireguard Stop" "$@"
}

chk_zone() {
	local config=$1
	
	config_get src $config src
	config_get dest $config dest
	if [ $src = "lan" -a $dest = "wg" ]; then
		uci set firewall."$config".dest="wan"
		uci commit firewall
	fi
}

WG=$1

forward=$(uci -q get wireguard."$WG".forward)
if [ "$forward" != "0" ]; then
	config_load firewall
	config_foreach chk_zone forwarding
else
	uci delete firewall.wgwforward
	uci delete firewall.wwgforward
	uci delete firewall.lwgforward
	uci delete firewall.wglforward
	uci commit firewall
fi
/etc/init.d/firewall restart

SERVE=$(uci get wireguard."$WG".client)
if [ $SERVE = "0" ]; then
	ifdown wg1
	uci set wireguard.settings.server="0"
	uci delete network.wg1
	uci set network.wg1=interface
	uci set network.wg1.proto="wireguard"
	uci set network.wg1.auto="0"
	uci set network.wg1.private_key=""
	uci set network.wg1.listen_port=""
	uci add_list network.wg1.addresses=""
	uci commit network
else
	INTER=$(uci -q get wireguard."$WG".wginter)
	if [ -z "$INTER" ]; then
		INTER=0
	fi
	ifdown wg$INTER
	uci set wireguard.settings.client="0"
	uci delete network.wg$INTER
	uci set network.wg$INTER=interface
	uci set network.wg$INTER.proto="wireguard"
	uci set network.wg$INTER.auto="0"
	uci set network.wg$INTER.private_key=""
	uci set network.wg$INTER.listen_port=""
	uci add_list network.wg$INTER.addresses=""
	uci commit network
fi
UDP=$(uci get wireguard."$WG".udptunnel)
if [ $UDP = 1 ]; then
	PID=$(ps |grep "udptunnel" | grep -v grep |head -n 1 | awk '{print $1}')
	kill -9 $PID
fi

uci set wireguard."$WG".active="0"
uci commit wireguard

/etc/init.d/wireguard stop