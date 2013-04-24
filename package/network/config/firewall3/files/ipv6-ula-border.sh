#!/bin/sh
ULA_PREFIX=$(uci -q get network.globals.ula_prefix)
[ -n "$ULA_PREFIX" ] || exit 0

ip6tables -I delegate_forward -s $ULA_PREFIX -m comment --comment "Enforce ULA-Border" -j zone_wan_dest_REJECT
ip6tables -I delegate_forward -d $ULA_PREFIX -m comment --comment "Enforce ULA-Border" -j zone_wan_dest_REJECT
