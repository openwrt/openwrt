#!/bin/sh

. /lib/functions.sh

port_mirror() {
	for lan in $(ls -d /sys/class/net/lan* | cut -dn -f3 |sort -n); do
		tc qdisc del dev lan$lan clsact
	done

	monitor=$(uci get switch.mirror.monitor)
	analysis=$(uci get switch.mirror.analysis)

	[ -n "$monitor" -a -n "$analysis" ] || return

	ifconfig $analysis up
	for port in $monitor; do
		tc qdisc add dev $port clsact
		tc filter add dev $port ingress matchall skip_sw action mirred egress mirror dev $analysis
		tc filter add dev $port egress matchall skip_sw action mirred egress mirror dev $analysis
	done
}

port_mirror
