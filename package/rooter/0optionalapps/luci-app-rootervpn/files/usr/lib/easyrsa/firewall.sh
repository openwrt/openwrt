#!/bin/sh 

log() {
	logger -t "Firewall Settings : " "$@"
}

sleep 5
VPN2LAN=$(uci get openvpn.settings.vpn2lan)
if [ -z $VPN2LAN ]; then
	VPN2LAN="0"
fi
VPNS2LAN=$(uci get openvpn.settings.vpns2lan)
if [ -z $VPNS2LAN ]; then
	VPNS2LAN="0"
fi
VPN2WAN=$(uci get openvpn.settings.vpn2wan)
if [ -z $VPN2WAN ]; then
	VPN2WAN="0"
fi

CHANGE="0"
if [ $VPN2LAN = "1" ]; then
	WW=$(uci get firewall.vpnforward1)
	if [ -z $WW ]; then
		uci set firewall.vpnforward1=forwarding
		uci set firewall.vpnforward1.dest="lan"
		uci set firewall.vpnforward1.src="VPN"
		CHANGE="1"
	fi
else
	WW=$(uci get firewall.vpnforward1)
	if [ ! -z $WW ]; then
		uci delete firewall.vpnforward1
		CHANGE="1"
	fi
fi

if [ $VPNS2LAN = "1" ]; then
	WW=$(uci get firewall.vpnforwards1)
	if [ -z $WW ]; then
		uci set firewall.vpnforwards1=forwarding
		uci set firewall.vpnforwards1.dest="lan"
		uci set firewall.vpnforwards1.src="VPNS"
		CHANGE="1"
	fi
else
	WW=$(uci get firewall.vpnforwards1)
	if [ ! -z $WW ]; then
		uci delete firewall.vpnforwards1
		CHANGE="1"
	fi
fi

if [ $VPN2WAN = "1" ]; then
	WW=$(uci get firewall.vpnforward2)
	if [ -z $WW ]; then
		uci set firewall.vpnforward2=forwarding
		uci set firewall.vpnforward2.dest="wan"
		uci set firewall.vpnforward2.src="VPNS"
		CHANGE="1"
	fi
else
	WW=$(uci get firewall.vpnforward2)
	if [ ! -z $WW ]; then
		uci delete firewall.vpnforward2
		CHANGE="1"
	fi
fi

if [ $CHANGE = "1" ]; then
	uci commit firewall
	/etc/init.d/firewall restart
fi