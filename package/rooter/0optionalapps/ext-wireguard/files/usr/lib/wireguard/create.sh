#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "Wireguard Conf" "$@"
}

WG=$(cat /tmp/wginst)

do_create() {
	local config=$1

	config_get name $config name
	if [ -z $name ]; then
		name=$config
	fi

	echo "----Start Conf File for "$name" ----" >> ${PKI_DIR}/package/wg.conf
	echo "[Interface]" >> ${PKI_DIR}/package/wg.conf
	config_get privatekey $config privatekey
	echo "PrivateKey = "$privatekey >> ${PKI_DIR}/package/wg.conf
	config_get address $config address
	echo "Address = "$address >> ${PKI_DIR}/package/wg.conf
	config_get endpoint_port $config endpoint_port
	if [ ! -z $endpoint_port ]; then
		echo "ListenPort = "$endpoint_port >> ${PKI_DIR}/package/wg.conf
	fi
	config_get dns $config dns
	if [ ! -z $dns ]; then
		echo "DNS = "$dns >> ${PKI_DIR}/package/wg.conf
	fi
	config_get mtu $config mtu
	if [ ! -z $mtu ]; then
		echo "MTU = "$mtu >> ${PKI_DIR}/package/wg.conf
	fi
	config_get wginter $config wginter
	if [ -z"$wginter"]; then
		wginter=0
	fi
	echo "PrivateKey = "$wginter >> ${PKI_DIR}/package/wg.conf
	echo " " >> ${PKI_DIR}/package/wg.conf
	echo "[Peer]" >> ${PKI_DIR}/package/wg.conf
	PUB=$(uci get wireguard."$WG".publickey)
	echo "PublicKey = "$PUB >> ${PKI_DIR}/package/wg.conf
	USE=$(uci get wireguard."$WG".usepre)
	if [ $USE = "1" ]; then
		PRE=$(uci get wireguard."$WG".presharedkey)
		echo "PresharedKey = "$PRE >> ${PKI_DIR}/package/wg.conf
	fi
	HOST=$(uci get wireguard."$WG".endpoint_host)
	PORT=$(uci get wireguard."$WG".port)
	if [ ! -z $PORT ]; then
		HOST=$HOST":"$PORT
	fi
	echo "Endpoint = "$HOST >> ${PKI_DIR}/package/wg.conf
	config_get allowed_ips $config allowed_ips
	echo "AllowedIPs = "$allowed_ips >> ${PKI_DIR}/package/wg.conf
	echo "----EndConf File for "$name" ----" >> ${PKI_DIR}/package/wg.conf
	echo " " >> ${PKI_DIR}/package/wg.conf
}

#PKI_DIR="/tmp/wireguard"
PKI_DIR="/www"
#rm -rfv "$PKI_DIR"
#mkdir -p ${PKI_DIR}
#chmod -R 0777 ${PKI_DIR}
cd ${PKI_DIR}
mkdir -p package
cd ..
chmod -R 0777 ${PKI_DIR}/package
#rm -rfv "/www/package"
#ln -s ${PKI_DIR}/package /www/package


rm -f ${PKI_DIR}/package/wg.conf
config_load wireguard
config_foreach do_create custom$WG

cd ${PKI_DIR}/package

tar -czf wgconf.tar.gz wg.conf
