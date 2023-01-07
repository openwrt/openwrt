#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "Wireguard KeyGen" "$@"
}

WG=$1
echo "$WG" > /tmp/wginst

sleep 5

EXST=$(uci get wireguard."$WG")
if [ -z $EXST ]; then
	uci set wireguard."$WG"="wireguard"
	uci commit wireguard
fi

PRIV=$(uci get wireguard."$WG".privatekey)
if [ -z $PRIV ]; then
	umask u=rw,g=,o=
	wg genkey | tee /tmp/wgserver.key | wg pubkey > /tmp/wgclient.pub
	wg genpsk > /tmp/wg.psk
	 
	WG_KEY="$(cat /tmp/wgserver.key)" # private key
	WG_PSK="$(cat /tmp/wg.psk)" # shared key
	WG_PUB="$(cat /tmp/wgclient.pub)" # public key to be used on other end
	rm -f /tmp/wgserver.key
	rm -f /tmp/wg.psk
	rm -f /tmp/wgclient.pub
	uci set wireguard."$WG".privatekey=$WG_KEY
	uci set wireguard."$WG".publickey=$WG_PUB
	uci set wireguard."$WG".presharedkey=$WG_PSK
	uci commit wireguard
fi

do_custom() {
	local config=$1

	config_get privatekey $config privatekey
	if [ -z "$privatekey" ]; then
		umask u=rw,g=,o=
		wg genkey | tee /tmp/wgserver.key | wg pubkey > /tmp/wgclient.pub
		wg genpsk > /tmp/wg.psk
		 
		WG_KEY="$(cat /tmp/wgserver.key)" # private key
		WG_PSK="$(cat /tmp/wg.psk)" # shared key
		WG_PUB="$(cat /tmp/wgclient.pub)" # public key to be used on other end
		rm -f /tmp/wgserver.key
		rm -f /tmp/wg.psk
		rm -f /tmp/wgclient.pub
				log "$WG_KEY"
		uci set wireguard."$config".privatekey=$WG_KEY
		uci set wireguard."$config".publickey=$WG_PUB
		uci set wireguard."$config".presharedkey=$WG_PSK
		uci set wireguard."$config".persistent_keepalive='25'
		uci set wireguard."$config".route_allowed_ips='1'
	fi
}

config_load wireguard
config_foreach do_custom custom$WG
uci commit wireguard