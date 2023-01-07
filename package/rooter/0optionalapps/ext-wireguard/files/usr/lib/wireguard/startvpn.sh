#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "Wireguard Start" "$@"
}

WG=$1

chk_zone() {
	local config=$1
	
	config_get src $config src
	config_get dest $config dest
	if [ $src = "lan" -a $dest = "wan" ]; then
		uci set firewall."$config".dest="wg"
		uci commit firewall
	fi
}

do_dns() {
	cdns=$1
	local ifce=$2
	ldns=$(uci -q get network.wg$ifce.dns)
	ex=$(echo "$ldns" | grep "$cdns")
	if [ -z $ex ]; then
		log "Add DNS $cdns to WG$ifce"
		uci add_list network.wg$ifce.dns="$cdns"
		uci commit network
		/etc/init.d/network reload
	fi
}

do_port() {
	PORT=$1
	udp=$2
	# look for rule for this port
	INB="inbound"$PORT$udp
	RULE=$(uci -q get firewall.$INB)
	if [ -z $RULE ]; then
		uci set firewall.$INB=rule
		uci set firewall.$INB.name=$INB
		uci set firewall.$INB.target=ACCEPT
		uci set firewall.$INB.src=*
		uci set firewall.$INB.proto=$udp
		uci set firewall.$INB.dest_port=$PORT
		uci commit firewall
		/etc/init.d/firewall reload
	fi
}

do_delete() {
	local config=$1
	
	uci delete network.$1
}

create_speer() {
	local config=$1

	uci set network.$config="wireguard_wg1"

	config_get persistent_keepalive $config persistent_keepalive
	uci set network.$config.persistent_keepalive="$persistent_keepalive"
	config_get route_allowed_ips $config route_allowed_ips
	uci set network.$config.route_allowed_ips="$route_allowed_ips"
	config_get publickey $config publickey
	uci set network.$config.public_key="$publickey"
	usepre=$(uci -q get wireguard.$WG.usepre)
	log "$usepre"
	if [ $usepre = "1" ]; then
		presharedkey=$(uci -q get wireguard.$WG.presharedkey)
		log "$presharedkey"
		uci set network.$config.preshared_key="$presharedkey"
	fi
	config_get allowed_ips $config allowed_ips
	allowed_ips=$allowed_ips","
	ips=$(echo $allowed_ips | cut -d, -f1)
	i=1
	while [ ! -z $ips ]
	do
		uci add_list network.$config.allowed_ips="$ips"
		i=$((i+1))
		ips=$(echo $allowed_ips | cut -d, -f$i)
	done

}

create_cpeer() {
	local config=$1
	local ifce=$2
	
	uci set network.$config="wireguard_wg$ifce"

	publickey=$(uci -q get wireguard."$config".publickey)
	uci set network.$config.public_key="$publickey"
	presharedkey=$(uci -q get wireguard."$WG".presharedkey)
	if [ ! -z $presharedkey ]; then
		uci set network.$config.preshared_key="$presharedkey"
	fi
	persistent_keepalive=$(uci -q get wireguard."$config".persistent_keepalive)
	if [ -z $persistent_keepalive ]; then
		persistent_keepalive=25
	fi
	uci set network.$config.persistent_keepalive="$persistent_keepalive"
	route_allowed_ips=1
	uci set network.$config.route_allowed_ips="$route_allowed_ips"
	
	if [ $UDP = 1 ]; then
		endpoint_host="127.0.0.1"
		uci set network.$config.endpoint_host="$endpoint_host"
		sport=$(uci -q get wireguard."$config".port)
		if [ -z $sport ]; then
			sport="54321"
		fi
		uci set network.$config.endpoint_port="$sport"
	else
		endpoint_host=$(uci -q get wireguard."$config".endpoint_host)
		uci set network.$config.endpoint_host="$endpoint_host"
		sport=$(uci -q get wireguard."$config".sport)
		if [ -z $sport ]; then
			sport="51280"
		fi
		uci set network.$config.endpoint_port="$sport"
	fi
	
	ips=$(uci -q get wireguard."$config".ips)","
	cips=$(echo $ips | cut -d, -f1)
	i=1
	while [ ! -z $cips ]
	do
		uci add_list network.$config.allowed_ips="$cips"
		i=$((i+1))
		cips=$(echo $ips | cut -d, -f$i)
	done
}

handle_server() {
	config_foreach do_delete wireguard_wg1
	
	uci delete network.wg1
	uci set network.wg1="interface"
	uci set network.wg1.proto="wireguard"
	
	auto=$(uci -q get wireguard."$WG".auto)
	if [ -z $auto ]; then
		auto="0"
	fi
	uci set network.wg1.auto="$auto"
	
	port=$(uci -q get wireguard."$WG".port)
	if [ -z $port ]; then
		port="51280"
	fi
	uci set network.wg1.listen_port="$port"
	do_port $port udp
	
	privatekey=$(uci -q get wireguard."$WG".privatekey)
	uci set network.wg1.private_key="$privatekey"

	ips=$(uci -q get wireguard."$WG".addresses)","
	cips=$(echo $ips | cut -d, -f1)
	i=1
	while [ ! -z $cips ]
	do
		uci add_list network.wg1.addresses="$cips"
		i=$((i+1))
		cips=$(echo $ips | cut -d, -f"$i")
		if [ -z $cips ]; then
			break
		fi
	done
	
	config_load wireguard
	config_foreach create_speer custom$WG
	
	uci commit network
}

handle_client() {
	ifce=$1
	config_foreach do_delete wireguard_wg$ifce
	
	uci delete network.wg$ifce
	uci set network.wg$ifce="interface"
	uci set network.wg$ifce.proto="wireguard"
	
	auto=$(uci -q get wireguard."$WG".auto)
	if [ -z $auto ]; then
		auto="0"
	fi
	uci set network.wg$ifce.auto="$auto"
	mtu=$(uci -q get wireguard."$WG".mtu)
	if [ ! -z $mtu ]; then
		uci set network.wg$ifce.mtu="$mtu"
	fi
	dns=$(uci -q get wireguard."$WG".dns)
	if [ ! -z $dns ]; then
		do_dns $dns $ifce
	fi
	port=$(uci -q get wireguard."$WG".port)
	if [ -z $port ]; then
		port="51280"
	fi
	uci set network.wg$ifce.listen_port="$port"
	do_port $port udp
	
	privatekey=$(uci -q get wireguard."$WG".privatekey)
	uci set network.wg$ifce.private_key="$privatekey"

	ips=$(uci -q get wireguard."$WG".addresses)","
	cips=$(echo $ips | cut -d, -f1)
	i=1
	while [ ! -z "$cips" ]
	do
		uci add_list network.wg$ifce.addresses="$cips"
		i=$((i+1))
		cips=$(echo "$ips" | cut -d, -f"$i")
		if [ -z "$cips" ]; then
			break
		fi
	done
	uci add_list network.wg$ifce.addresses="::/0"
	
	create_cpeer $WG $ifce
	
	uci commit network
}

udp_server() {
	local config=$1
	udpport=$(uci -q get wireguard."$WG".udpport)
	if [ -z $udpport ]; then
		udpport="54321"
	fi
	port=$(uci -q get wireguard."$WG".port)
	if [ -z $port ]; then
		port="54321"
	fi
	do_port $udpport tcp
	udptunnel -s -v "0.0.0.0:"$udpport "127.0.0.1:"$port &
	#log "udptunnel -s -v 0.0.0.0:$udpport 127.0.0.1:$port"
}

udp_client() {
	local config=$1
	port=$(uci -q get wireguard."$WG".port)
	if [ -z $port ]; then
		port="54321"
	fi
	endpoint_host=$(uci -q get wireguard.$WG.endpoint_host)
	sport=$(uci -q get wireguard.$WG.sport)
	if [ -z $sport ]; then
		sport="51280"
	fi
	
	udptunnel "127.0.0.1:"$port $endpoint_host":"$sport &
	#log "udptunnel 127.0.0.1:$port $endpoint_host:$sport"
}

forward=$(uci -q get wireguard."$WG".forward)
if [ "$forward" != "0" ]; then
	config_load firewall
	config_foreach chk_zone forwarding
else
	uci set firewall.wgwforward=forwarding
	uci set firewall.wgwforward.dest="wan"
	uci set firewall.wgwforward.src="wg"
	
	uci set firewall.wwgforward=forwarding
	uci set firewall.wwgforward.dest="wg"
	uci set firewall.wwgforward.src="wan"
	
	uci set firewall.lwgforward=forwarding
	uci set firewall.lwgforward.dest="wg"
	uci set firewall.lwgforward.src="lan"
	
	uci set firewall.wglforward=forwarding
	uci set firewall.wglforward.dest="lan"
	uci set firewall.wglforward.src="wg"
	uci commit firewall
fi
etc/init.d/firewall restart

config_load network
SERVE=$(uci -q get wireguard."$WG".client)
if [ $SERVE = "0" ]; then
	running=$(uci -q get wireguard.settings.server)
	if [ $running = 1 ]; then
		exit 0
	fi
	UDP=$(uci -q get wireguard."$WG".udptunnel)
	if [ $UDP = 1 ]; then
		udp_server $WG
	fi
	handle_server
	uci commit network
	ifup wg1
	sleep 2
	uci set wireguard.settings.server="1"
else
	running=$(uci -q get wireguard.settings.client)
	log "Client running $running"
	
	INTER=$(uci -q get wireguard."$WG".wginter)
	if [ -z "$INTER" ]; then
		INTER=0
	fi
	UDP=$(uci -q get wireguard."$WG".udptunnel)
	if [ $UDP = 1 ]; then
		udp_client $WG
	fi
	handle_client $INTER
	uci commit network
	log "Start Interface"
	ifup wg$INTER
	sleep 2
	uci set wireguard.settings.client="1"
fi

uci set wireguard."$WG".active="1"
uci commit wireguard

