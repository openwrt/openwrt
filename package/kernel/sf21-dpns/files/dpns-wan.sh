#!/bin/sh
# Tell the DPNS NAT engine which devices are NAT uplinks (WAN). Without this the
# NPU does not match traffic arriving on WAN against reply (DNAT) entries and
# the WAN->LAN direction stays on the CPU. Uses the devices of all firewall
# zones with masquerading enabled (up to 8).
[ -w /proc/dpns_nat ] || exit 0
. /lib/functions.sh

devs=""
add_dev() {
	[ -n "$1" ] || return
	case " $devs " in *" $1 "*) return ;; esac
	devs="$devs $1"
}
zone_cb() {
	local masq net n
	config_get_bool masq "$1" masq 0
	[ "$masq" = 1 ] || return
	config_get net "$1" network
	for n in $net; do add_dev "$(uci -q get network.$n.device)"; done
}
config_load firewall
config_foreach zone_cb zone

i=0
for d in $devs; do
	[ $i -lt 8 ] || break
	echo "subnet is_get 0 is_lan 0 index $i ifname $d" > /proc/dpns_nat
	i=$((i + 1))
done
while [ $i -lt 8 ]; do
	echo "subnet is_get 0 is_lan 0 index $i ifname -" > /proc/dpns_nat
	i=$((i + 1))
done
