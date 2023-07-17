#!/bin/sh

packet_steering="$(uci -q get network.@globals[0].packet_steering)"
flow_offloading="$(uci -q get firewall.@defaults[0].flow_offloading)"
flow_offloading_hw="$(uci -q get firewall.@defaults[0].flow_offloading_hw)"

[ "$packet_steering" = 1 ] && {
	if [ ${flow_offloading_hw:-0} -gt 0 ]; then
		# HW offloading
		# Not implemented
		:
	elif [ ${flow_offloading:-0} -gt 0 ]; then
		# SW offloading
		:
	else
		# Default
		# LAN
	    for q in $(ls /sys/class/net/lan*/queues/rx-*/rps_cpus); do echo f > $q; done
	    for q in $(ls /sys/class/net/lan*/queues/tx-*/xps_cpus); do echo f > $q; done
	    for q in $(ls /sys/class/net/lan*/queues/rx-*/rps_flow_cnt); do echo 4096 > $q; done
	    # WAN
	    for q in $(ls /sys/class/net/wan/queues/rx-*/rps_cpus); do echo f > $q; done
	    for q in $(ls /sys/class/net/wan/queues/tx-*/xps_cpus); do echo f > $q; done
	    for q in $(ls /sys/class/net/wan/queues/rx-*/rps_flow_cnt); do echo 4096 > $q; done

	    echo 32768 > /proc/sys/net/core/rps_sock_flow_entries
	fi
}

# Enable threaded network backlog processing
echo 1 > /proc/sys/net/core/backlog_threaded
