#!/bin/ash

alias debug=${DEBUG:-:}

# allow env to override nvram
nvram () {
  if [ -x /usr/sbin/nvram ]; then
    case $1 in
      get) eval "echo \${$2:-\$(command nvram get $2)}";;
      *) command nvram $*;;
    esac
  else
    case $1 in
      get) eval "echo \${$2:-\${DEFAULT_$2}}";;
      *);;
    esac
  fi  
}

# valid interface?
if_valid () (
  ifconfig "$1" >&- 2>&- ||
  [ "${1%%[0-9]}" = "br" ] ||
  {
    [ "${1%%[0-9]}" = "vlan" ] && ( 
      i=${1#vlan}
      hwname=$(nvram get vlan${i}hwname)
      hwaddr=$(nvram get ${hwname}macaddr)
      [ -z "$hwaddr" ] && return 1

      vif=$(ifconfig -a | awk '/^eth.*'$hwaddr'/ {print $1; exit}' IGNORECASE=1)
      debug "# vlan$i => $vif"

      $DEBUG ifconfig $vif up
      $DEBUG vconfig add $vif $i 2>&-
    )
  } ||
  { debug "# missing interface '$1' ignored"; false; }
)

bitcount () {
  local c=$1
  echo $((
  c=((c>> 1)&0x55555555)+(c&0x55555555),
  c=((c>> 2)&0x33333333)+(c&0x33333333),
  c=((c>> 4)&0x0f0f0f0f)+(c&0x0f0f0f0f),
  c=((c>> 8)&0x00ff00ff)+(c&0x00ff00ff),
  c=((c>>16)&0x0000ffff)+(c&0x0000ffff)
  ))
}

valid_netmask () {
  return $((-($1)&~$1))
}

ip2int () (
  set $(echo $1 | tr '\.' ' ')
  echo $(($1<<24|$2<<16|$3<<8|$4))
)

int2ip () {
  echo $(($1>>24&255)).$(($1>>16&255)).$(($1>>8&255)).$(($1&255))
}
