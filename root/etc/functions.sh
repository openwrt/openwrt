#!/bin/ash

alias debug=${DEBUG:-:}

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

    vif=$(ifconfig -a | awk '/^eth.*'$hwaddr'/ {print $1; exit}' IGNORECASE=1)
    debug "# vlan$i: $hwname $hwaddr => $vif"

    $DEBUG ifconfig $vif up
    $DEBUG vconfig add $vif $i 2>/dev/null
  }
  ifconfig "$1" >/dev/null 2>&1 || [ "${1%%[0-9]}" = "br" ]
)
