#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "Guest Wifi" "$@"
}

do_radio() {
	local config=$1
	local freq=$2
	local channel

	config_get channel $1 channel
	if [ $freq = "0" ]; then
		if [ $channel -lt 15 ]; then
			RADIO=$config
		fi
	else
		if [ $channel -gt 15 ]; then
			RADIO=$config
		fi
	fi
}

do_check() {
	local config=$1
	local wfreq=$2
	local enabled
	local freq

	config_get enabled $1 enabled
	if [ $enabled = "1" ]; then
		config_get freq $1 freq
		if [ $freq = $wfreq ]; then
			CHECK=1
		fi
	fi
}

NAME=$1
RAD=$(uci -q get guestwifi.$NAME.freq)
CHECK=0
config_load guestwifi
config_foreach do_check guestwifi $RAD
if [ $CHECK -eq 1 ]; then
	exit 0
fi

config_load wireless
config_foreach do_radio wifi-device $RAD
GUEST="guest""$RADIO"

LANIP=$(uci -q get network.lan.ipaddr)
L1=$(echo $LANIP | cut -d. -f1)
L2=$(echo $LANIP | cut -d. -f2)
L3=$(echo $LANIP | cut -d. -f3)
L4=$(echo $LANIP | cut -d. -f4)
NL3="254"
if [ $RAD = "0" ]; then
	NL3="253"
fi
if [ $L3 = "254" ]; then
	NL3="1"
fi
if [ $L3 = "253" ]; then
	NL3="2"
fi
IP="$L1"."$L2"."$NL3"."$L4"

ifname1="ifname"
if [ -e /etc/newstyle ]; then
	ifname1="device"
fi
	
WW=$(uci get -q network.$GUEST)
if [ -z $WW ]; then
# Configure guest network
	uci delete network.${GUEST}_dev
	uci set network.${GUEST}_dev=device
	uci set network.${GUEST}_dev.type=bridge
	uci set network.${GUEST}_dev.name="br-"$GUEST

	uci delete network.$GUEST
	uci set network.$GUEST=interface
	uci set network.$GUEST.proto=static
	uci set network.$GUEST.ipaddr=$IP
	uci set network.$GUEST.$ifname1=br-${GUEST}
	uci set network.$GUEST.netmask=255.255.255.0
	
# Configure DHCP for guest network
	uci delete dhcp.$GUEST
	uci set dhcp.$GUEST=dhcp
	uci set dhcp.$GUEST.interface=$GUEST
	uci set dhcp.$GUEST.start=50
	uci set dhcp.$GUEST.limit=200
	uci set dhcp.$GUEST.leasetime=1h
	 
# Configure firewall for guest network
	## Configure guest zone
	uci delete firewall.$GUEST"_zone"
	uci set firewall.$GUEST"_zone"=zone
	uci set firewall.$GUEST"_zone".name=$GUEST
	uci set firewall.$GUEST"_zone".network=$GUEST
	uci set firewall.$GUEST"_zone".input=REJECT
	uci set firewall.$GUEST"_zone".forward=REJECT
	uci set firewall.$GUEST"_zone".output=ACCEPT
	## Allow Guest -> Internet
	uci delete firewall.$GUEST"_forwarding"
	uci set firewall.$GUEST"_forwarding"=forwarding
	uci set firewall.$GUEST"_forwarding".src=$GUEST
	uci set firewall.$GUEST"_forwarding".dest=wan
	## Allow DNS Guest -> Router
	uci delete firewall.$GUEST"_rule_dns"
	uci set firewall.$GUEST"_rule_dns"=rule
	uci set firewall.$GUEST"_rule_dns".name="Allow "$GUEST" DNS Queries"
	uci set firewall.$GUEST"_rule_dns".src=$GUEST
	uci set firewall.$GUEST"_rule_dns".dest_port=53
	uci set firewall.$GUEST"_rule_dns".proto=tcpudp
	uci set firewall.$GUEST"_rule_dns".target=ACCEPT
	## Allow DHCP Guest -> Router
	uci delete firewall.$GUEST"_rule_dhcp"
	uci set firewall.$GUEST"_rule_dhcp"=rule
	uci set firewall.$GUEST"_rule_dhcp".name="Allow "$GUEST" DHCP request"
	uci set firewall.$GUEST"_rule_dhcp".src=$GUEST
	uci set firewall.$GUEST"_rule_dhcp".src_port=68
	uci set firewall.$GUEST"_rule_dhcp".dest_port=67
	uci set firewall.$GUEST"_rule_dhcp".proto=udp
	uci set firewall.$GUEST"_rule_dhcp".target=ACCEPT

	uci commit
fi

# Configure guest Wi-Fi
SSID=$(uci -q get guestwifi.$NAME.ssid)
ENCR=$(uci -q get guestwifi.$NAME.encrypted)

uci delete wireless.$NAME
uci set wireless.$NAME=wifi-iface
uci set wireless.$NAME.device=$RADIO
uci set wireless.$NAME.mode=ap
uci set wireless.$NAME.network=$GUEST
uci set wireless.$NAME.ssid=$SSID
case $ENCR in
	"0" )
		uci set wireless.$NAME.encryption="none"
		uci set wireless.$NAME.key=""
	;;
	"1" )
		uci set wireless.$NAME.encryption="psk"
		uci set wireless.$NAME.key=$(uci get guestwifi.$NAME.password)
	;;
	"2" )
		uci set wireless.$NAME.encryption="psk2"
		uci set wireless.$NAME.key=$(uci get guestwifi.$NAME.password)
	;;
esac
uci commit wireless

QOS=$(uci -q get guestwifi.$NAME.qos)

if [ $QOS = "1" ]; then
	DL=$(uci -q get guestwifi.$NAME.dl)
	let "DL=$DL * 1000"
	UL=$(uci -q get guestwifi.$NAME.ul)
	let "UL=$UL * 1000"
	IFACE="$(iwinfo | grep "ESSID" | grep $SSID)"
	WI=${IFACE% *}
	WI=${WI% *}
	uci delete sqm.$NAME
	uci set sqm.$NAME=queue
	uci set sqm.$NAME.interface=$WI
	uci set sqm.$NAME.enabled='1'
	uci set sqm.$NAME.upload=$DL
	uci set sqm.$NAME.download=$UL
	uci set sqm.$NAME.qdisc='cake'
	uci set sqm.$NAME.script='piece_of_cake.qos'
	uci set sqm.$NAME.qdisc_advanced='1'
	uci set sqm.$NAME.linklayer='none'
	uci set sqm.$NAME.ingress_ecn='ECN'
	uci set sqm.$NAME.egress_ecn='ECN'
	uci set sqm.$NAME.debug_logging='0'
	uci set sqm.$NAME.verbosity='5'
	uci set sqm.$NAME.squash_dscp='1'
	uci set sqm.$NAME.squash_ingress ='1'
	uci commit sqm
	/etc/init.d/sqm start
	/etc/init.d/sqm enable
fi


if [ -z $WW ]; then
	/etc/init.d/dnsmasq restart
	/etc/init.d/firewall restart
	/etc/init.d/network restart
fi



uci set guestwifi.$NAME.enabled="1"
uci commit guestwifi