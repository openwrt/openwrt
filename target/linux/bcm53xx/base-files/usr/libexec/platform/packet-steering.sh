#!/bin/sh

packet_steering="$(uci -q get network.@globals[0].packet_steering)"
flow_offloading="$(uci -q get firewall.@defaults[0].flow_offloading)"
flow_offloading_hw="$(uci -q get firewall.@defaults[0].flow_offloading_hw)"

[ "$packet_steering" != 1 ] && {
	echo 0 > /sys/class/net/br-lan/queues/rx-0/rps_cpus
	echo 0 > /sys/class/net/eth0/queues/rx-0/rps_cpus
	exit 0
}

if [ ${flow_offloading_hw:-0} -gt 0 ]; then
	# HW offloading
	echo 0 > /sys/class/net/br-lan/queues/rx-0/rps_cpus
	echo 0 > /sys/class/net/eth0/queues/rx-0/rps_cpus
elif [ ${flow_offloading:-0} -gt 0 ]; then
	# SW offloading
	# br-lan setup doesn't seem to matter for offloading case
	echo 2 > /sys/class/net/eth0/queues/rx-0/rps_cpus
else
	# Default
	echo 2 > /sys/class/net/br-lan/queues/rx-0/rps_cpus
	echo 0 > /sys/class/net/eth0/queues/rx-0/rps_cpus
fi
