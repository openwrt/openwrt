#!/bin/sh

debug () {
 [ -z "$DEBUG" ] || echo $1
}

# allow env to override nvram 
nvram_get () {
 eval "echo \${$1:-\$(nvram get $1)}"
}
. /etc/nvram.overrides

# valid interface?
if_valid () (
  [ "${1%%[0-9]}" = "vlan" ] && {
    i=${1#vlan} 
    hwname=$(nvram_get vlan${i}hwname)
    hwaddr=$(nvram_get ${hwname}macaddr)
    [ -z "$hwaddr" ] && return 1

    vif=$(ifconfig -a | awk '{IGNORECASE=1} /^eth.*'$hwaddr'/ {print $1; exit}')
    debug "# vlan$i: $hwname $hwaddr => $vif"

    $DEBUG ifconfig $vif up
    $DEBUG vconfig add $vif $i 2>/dev/null
  }
  ifconfig "$1" >/dev/null 2>&1 || [ "${1%%[0-9]}" = "br" ] 
  return $?
)

mac () {
  echo $2|awk -F ":" '{for(x=6,y='$1';x;x--){y+=int("0x"$x);$x=sprintf("%02x",y%256);y/=256}gsub(" ",":");print$0}'
}                                                                                                                  
wifi () (
  debug "### wifi $1 ###"
  if=$(awk 'gsub(":","") {print $1}' /proc/net/wireless)
  $DEBUG ifconfig $if hw ether $(mac 2 $(nvram get et0macaddr))
  $DEBUG wlconf $if $1 
)

ifup () (
  type=$1
  debug "### ifup $type ###"
  
  if=$(nvram_get ${type}_ifname)
  if [ "${if%%[0-9]}" = "ppp" ]; then
    if=$(nvram_get pppoe_ifname) 
  fi
  
  if_valid $if || return
  
  $DEBUG ifconfig $if down
  if [ "${if%%[0-9]}" = "br" ]; then
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
    } done
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

      debug "# --- creating /etc/resolv.conf ---"
      for dns in $(nvram_get ${type}_dns); do {
	echo "nameserver $dns" >> /etc/resolv.conf
      } done
    ;;
    dhcp)
      pidfile=/tmp/dhcp-${type}.pid
      if [ -f $pidfile ]; then
        $DEBUG kill $(cat $pidfile)
      fi
      cmd="udhcpc -i $if -b -p $pidfile &"
      ${DEBUG:-eval} $cmd
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
      echo "### WARNING $if: $if_proto is not supported"
    ;;
  esac
)

ifdown () (                                                                       
  type=$1
  debug "### ifdown $type ###"
  if=$(nvram_get ${type}_ifname)                                                
  if_valid $if || return                                                        
  $DEBUG ifdown $if
)
