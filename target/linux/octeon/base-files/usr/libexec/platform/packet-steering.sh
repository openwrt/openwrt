#!/bin/sh
# Packet steering on OCTEON: off unless asked for explicitly.
#
# octeon_pip spreads the received flows over the CPUs in hardware (one
# POW receive group per CPU). The generic default (network.globals.packet_steering
# unset) points the RPS of every port at one CPU, which funnels all of a port's
# traffic through that CPU's backlog. Measured on an EdgeRouter 4 (CN7030,
# 4 cores, staging driver, same receive groups): 64 byte forwarding 450 -> 92
# kpps, WireGuard TCP 336 -> 165-259 Mbit/s; RPS over all CPUs was measured
# worse than none as well.
# The init script passes only network.globals.packet_steering to this hook,
# so read network.globals.steering_flows here.
case "$1" in
1|2)
	flows="$(uci -q get "network.@globals[0].steering_flows")"
	[ "${flows:-0}" -gt 0 ] && opts="-l $flows"
	exec /usr/libexec/network/packet-steering.uc $opts "$1"
	;;
*) exec /usr/libexec/network/packet-steering.uc 0 ;;
esac
