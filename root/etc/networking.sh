#!/bin/sh
# OpenWrt Networking script
# $Id$
# Copyright (c) 2004 Mike Baker <mbm at alt.org>

# to debug:
# export DEBUG=echo

export PATH=/usr/bin:/bin:/usr/sbin:/sbin

# lookup an interface by mac address
mac2if () {
  if=$(ifconfig -a | awk '{IGNORECASE=1} /^eth.*'$1'/ {print $1; exit}')
  echo $if
}

# allow env to override nvram 
nvram_get () {
 eval "echo \${$1:=\$(nvram get $1)}"
}

# valid interface?
if_valid () {
  [ "${1%[0-9]}" = "vlan" ] && {
    i=${1##vlan} 
    hwname=$(nvram_get vlan${i}hwname)
    hwaddr=$(nvram_get ${hwname}macaddr)
    [ -z "$hwaddr" ] && return 1

    vif=$(mac2if $hwaddr)
    echo "# vlan${i}: $hwname $hwaddr => $vif"

    $DEBUG ifconfig $vif up
    $DEBUG vconfig add $vif $i 2>/dev/null
  }
  ifconfig "$1" >/dev/null 2>&1 || [ "${1%[0-9]}" = "br" ] 
  return $?
}

wifi_init () {
  echo "# --- wifi init ---"
  # assume wifi is eth2, fall back to eth1
  if="eth2"
  if_valid $if || if="eth1"
  $DEBUG wlconf $if up
}

configure () {
  type=$1
  echo "# --- $type ---"
  
  if=$(nvram_get ${type}_ifname)
  if [ "${if%[0-9]}" = "ppp" ]; then
    if=$(nvram_get pppoe_ifname) 
  fi
  if_valid $if || return
  
  $DEBUG ifconfig $if down
  if [ "${if%[0-9]}" = "br" ]; then
    stp=$(nvram_get ${type}_stp)
    $DEBUG brctl delbr $if
    $DEBUG brctl addbr $if
    $DEBUG brctl setfd $if 0
    $DEBUG brctl stp $if $stp
    if_list=$(nvram_get ${type}_ifnames)
    for sif in $if_list; do {
      if_valid $sif || continue
      $DEBUG ifconfig $sif 0.0.0.0 up
      $DEBUG brctl addif $if $sif 
    }; done
  fi

  if_mac=$(nvram_get ${type}_hwaddr)
  [ -z "$if_mac" ] || $DEBUG ifconfig $if hw ether $if_mac
 
  if_proto=$(nvram_get ${type}_proto)
  case "$if_proto" in
    static)
      if_ip=$(nvram_get ${type}_ipaddr)
      if_netmask=$(nvram_get ${type}_netmask)
      if_gateway=$(nvram_get ${type}_gateway)
      
      ipcalc -s "$if_ip"      || return 
      ipcalc -s "$if_netmask" || return 
      $DEBUG ifconfig $if $if_ip netmask $if_netmask up

      ipcalc -s "$if_gateway" || return 
      $DEBUG route add default gw $if_gateway

      [ -f /etc/resolv.conf ] && return

      echo "# --- creating /etc/resolv.conf ---"
      for dns in $(nvram_get ${type}_dns); do {
	echo "nameserver $dns" >> /etc/resolv.conf
      }; done
    ;;
    dhcp)
      pidfile=/tmp/dhcp-${type}.pid
      if [ -f $pidfile ]; then
        $DEBUG kill $(cat $pidfile)
      fi
      $DEBUG udhcpc -i $if -b -p /tmp/dhcp-${type}.pid
    ;;
    pppoe)
      if_username=$(nvram_get ppp_username)
      if_password=$(nvram_get ppp_passwd)
      if_redial=$(nvram_get ppp_redialperiod)
      if_idletime=$(nvram_get ppp_idletime)
      
      $DEBUG ifconfig $if 0.0.0.0 up
      
      $DEBUG /sbin/pppoecd $if -u $if_username -p $if_password -i 0 -I $if_redial -T $if_idletime -k
    ;;
    *)
      echo "$if: $if_proto is not supported"
    ;;
  esac
}

### START NETWORKING ###
wifi_init

$DEBUG vconfig set_name_type VLAN_PLUS_VID_NO_PAD

# hacks for 1.x hardware
[ "$(nvram get boardnum)"  = "42" ] && \
[ "$(nvram get boardtype)" = "bcm94710dev" ] && {
  echo "# 1.x HACK"
  vlan1hwname="et0"
  vlan2hwname="et0"

  # we remap old device names to new
  # it's recommended that you continue to
  # use the old names to preserve backwards
  # compatibility
  remap () {
    eval $1=\"$(nvram_get $1 | awk '{
	  gsub(/eth0/,"vlan2")
	  gsub(/eth1/,"vlan1")
	  print $0
    }')\"
  }

  remap lan_ifname
  remap lan_ifnames
  remap wifi_ifname
  remap wifi_ifnames
  remap wan_ifname
  remap wan_ifnames
  remap pppoe_ifname
}

# failsafe if reset is held 
[ "$FAILSAFE" = "true" ] && {
  lan_ifname="br0"
  lan_ifnames="vlan0 vlan2 eth1 eth2 eth3"
  lan_ipaddr="192.168.1.1"
  lan_netmask="255.255.255.0"
  lan_hwaddr="00:0B:AD:0A:DD:00"
  wan_ifname="none"
  wifi_ifname="none"
}

# linksys bug has lan doing dhcp; force static
lan_proto="static"

configure lan
configure wifi
configure wan

for route in $(nvram_get static_route); do {
      ip=${route%%:*} route=${route#*:}
 netmask=${route%%:*} route=${route#*:}
 gateway=${route%%:*} route=${route#*:}
  metric=${route%%:*} route=${route#*:}
      if=${route%%:*}
  $DEBUG route add -net $ip netmask $netmask gw $gateway metric $metric dev $if
} done
