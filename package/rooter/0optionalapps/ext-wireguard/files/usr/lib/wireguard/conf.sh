#!/bin/sh

log() {
	modlog "Wireguard Conf" "$@"
}

name=$1
file=$2
auto=$3
if [ -z $auto ]; then
	auto="0"
fi

extract() {
	line=$1
	PD=$(echo "$line" | grep "#")
	if [ ! -z "$PD" ]; then
		return
	fi
	PRK=$(echo "$line" | grep "PrivateKey" | tr " " ",")
	if [ ! -z "$PRK" ]; then
		PrivateKey=$(echo $PRK | cut -d, -f3)
	fi
	PRK=$(echo "$line" | grep "PublicKey" | tr " " ",")
	if [ ! -z "$PRK" ]; then
		PublicKey=$(echo $PRK | cut -d, -f3)
	fi
	PRK=$(echo "$line" | grep "PresharedKey" | tr " " ",")
	if [ ! -z "$PRK" ]; then
		PreSharedKey=$(echo $PRK | cut -d, -f3)
	fi
	INTER=$(echo "$line" | grep "WGinterface" | tr " " ",")
	if [ ! -z "$INTER" ]; then
		wginter=$(echo $INTER | cut -d, -f3)
		if [ "$wginter" -gt 1 ]; then
			wginter="1"
		fi
	fi
	PRK=$(echo "$line" | grep "Address" | tr " " "#")
	if [ ! -z "$PRK" ]; then
		if [ -z $Address ]; then
			Address=$(echo $PRK | cut -d# -f3)
		else
			Address=$Address","$(echo $PRK | cut -d# -f3)
		fi
	fi
	PRK=$(echo "$line" | grep "dns" | tr " " "#")
	if [ ! -z "$PRK" ]; then
		dns=$(echo $PRK | cut -d# -f3)
	fi
	PRK=$(echo "$line" | grep "DNS" | tr " " "#")
	if [ ! -z "$PRK" ]; then
		dns=$(echo $PRK | cut -d# -f3)
	fi
	PRK=$(echo "$line" | grep "ListenPort" | tr " " ",")
	if [ ! -z "$PRK" ]; then
		listenport=$(echo $PRK | cut -d, -f3)
	fi
	PRK=$(echo "$line" | grep "AllowedIPs" | tr " " "#")
	if [ ! -z "$PRK" ]; then
		if [ -z $allowedips ]; then
			allowedips=$(echo $PRK | cut -d# -f3)
		else
			allowedips=$allowedips","$(echo $PRK | cut -d# -f3)
		fi
	fi
	PRK=$(echo "$line" | grep "Endpoint" | tr " " ",")
	if [ ! -z "$PRK" ]; then
		endpoint=$(echo $PRK | cut -d, -f3)
	fi
	MTU=$(echo "$line" | grep "MTU" | tr " " ",")
	if [ ! -z "$MTU" ]; then
		mtu=$(echo $MTU | cut -d, -f3)
	fi
}

listenport="51280"
dns=""
sed -i -e "s!PrivateKey= !PrivateKey=!g" $file
sed -i -e "s!PrivateKey=!PrivateKey = !g" $file
sed -i -e "s!PublicKey= !PublicKey=!g" $file
sed -i -e "s!PublicKey=!PublicKey = !g" $file
sed -i -e "s!PresharedKey= !PresharedKey=!g" $file
sed -i -e "s!PresharedKey=!PresharedKey = !g" $file
sed -i -e "s!Address= !Address=!g" $file
sed -i -e "s!Address=!Address = !g" $file
sed -i -e "s!WGinterface=!WGinterface = !g" $file
sed -i -e "s!WGinterface= !WGinterface = !g" $file
sed -i -e "s!dns= !dns=!g" $file
sed -i -e "s!dns=!dns = !g" $file
sed -i -e "s!DNS= !DNS=!g" $file
sed -i -e "s!DNS=!DNS = !g" $file
sed -i -e "s!ListenPort= !ListenPort=!g" $file
sed -i -e "s!ListenPort=!ListenPort = !g" $file
sed -i -e "s!AllowedIPs= !AllowedIPs=!g" $file
sed -i -e "s!AllowedIPs=!AllowedIPs = !g" $file
sed -i -e "s!Endpoint= !Endpoint=!g" $file
sed -i -e "s!Endpoint=!Endpoint = !g" $file
sed -i -e "s!MTU= !MTU=!g" $file
sed -i -e "s!MTU=!MTU = !g" $file

while IFS= read -r linex
do
	extract "$linex"
done < $file
extract "$linex"
PRK=$(echo "$endpoint" | tr ":" ",")
endpoint=$(echo $PRK | cut -d, -f1)
sport=$(echo $PRK | cut -d, -f2)
if [ -z "$wginter" ]; then
	wginter="0"
fi
uci delete wireguard.$name
uci set wireguard.$name=wireguard
uci set wireguard.$name.auto=$auto
uci set wireguard.$name.client="1"
uci set wireguard.$name.active="0"
uci set wireguard.$name.privatekey="$PrivateKey"
uci set wireguard.$name.presharedkey="$PreSharedKey"
uci set wireguard.$name.port="$listenport"
uci set wireguard.$name.addresses="$Address"	
uci set wireguard.$name.dns="$dns"
uci set wireguard.$name.wginter="$wginter"
uci set wireguard.$name.publickey="$PublicKey"
uci set wireguard.$name.endpoint_host="$endpoint"
uci set wireguard.$name.ips="$allowedips"
uci set wireguard.$name.name="$name"
uci set wireguard.$name.sport="$sport"
uci set wireguard.$name.mtu="$mtu"
uci set wireguard.$name.persistent_keepalive='25'
uci commit wireguard

rm -f $file
		