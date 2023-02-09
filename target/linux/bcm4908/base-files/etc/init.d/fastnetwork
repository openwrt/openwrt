#!/bin/sh /etc/rc.common

START=25
USE_PROCD=1

start_service() {
	reload_service
}

service_triggers() {
	procd_add_reload_trigger "network"
	procd_add_reload_trigger "firewall"
	procd_add_reload_interface_trigger "lan"
}

reload_service() {
	local packet_steering="$(uci -q get network.@globals[0].packet_steering)"
	local num_cpus="$(grep -c "^processor.*:" /proc/cpuinfo)"
	local flow_offloading="$(uci -q get firewall.@defaults[0].flow_offloading)"
	local flow_offloading_hw="$(uci -q get firewall.@defaults[0].flow_offloading_hw)"
	local rps_eth0=0
	local rps_br_lan=0

	[ "$num_cpus" -le 1 ] && return

	[ "$packet_steering" = 1 ] && {
		if [ ${flow_offloading_hw:-0} -gt 0 ]; then
			# HW offloading
			# Not implemented
			:
		elif [ ${flow_offloading:-0} -gt 0 ]; then
			# SW offloading
			# BCM4908 always reaches ~940 Mb/s
			:
		else
			# Default
			case "$num_cpus" in
				2) rps_eth0=2; rps_br_lan=2;;
				4) rps_eth0=e; rps_br_lan=e;;
			esac
		fi
	}

	echo $rps_eth0 > /sys/class/net/eth0/queues/rx-0/rps_cpus
	echo $rps_br_lan > /sys/class/net/br-lan/queues/rx-0/rps_cpus
}
