#!/bin/ash

alias debug=${DEBUG:-:}

# allow env to override nvram
nvram () {
  case $1 in
    get) eval "echo \${NVRAM_$2:-\$(command nvram get $2)}";;
    *) command nvram $*;;
  esac
}
. /etc/nvram.overrides

# valid interface?
if_valid () (
  [ "${1%%[0-9]}" = "vlan" ] && {
    i=${1#vlan}
    hwname=$(nvram get vlan${i}hwname)
    hwaddr=$(nvram get ${hwname}macaddr)
    [ -z "$hwaddr" ] && return 1

    vif=$(ifconfig -a | awk '/^eth.*'$hwaddr'/ {print $1; exit}' IGNORECASE=1)
    debug "# vlan$i: $hwname $hwaddr => $vif"

    $DEBUG ifconfig $vif up
    $DEBUG vconfig add $vif $i 2>/dev/null
  }
  ifconfig "$1" >/dev/null 2>&1 || [ "${1%%[0-9]}" = "br" ]
  [ $? = 0 ] || { echo -e "# $1 ignored: no such interface"; return 1; }
)
