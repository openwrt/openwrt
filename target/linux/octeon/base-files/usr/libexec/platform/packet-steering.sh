#!/bin/sh

. /lib/functions.sh

packet_steering="$1"
steering_flows="$(uci -q get 'network.@globals[0].steering_flows')"

if [ "$(board_name)" = 'paloaltonetworks,pa-220' ] &&
   [ "$(cat /sys/module/octeon_ethernet/parameters/receive_group_order 2>/dev/null)" = 2 ]; then
	awk '$NF == "Ethernet" && $(NF-2) == "CIU" {
		irq = $1; sub(/:$/, "", irq); hw = $(NF-1);
		if (hw >= 0 && hw < 4) print irq, hw;
		else if (hw == 53) print irq, 3;
	}' /proc/interrupts | while read -r irq cpu; do
		echo "$cpu" > "/proc/irq/$irq/smp_affinity_list"
	done
	# Hardware receive groups already distribute flows across the cores.
	[ -n "$packet_steering" ] || packet_steering=0
fi

if [ "${steering_flows:-0}" -gt 0 ]; then
	exec /usr/libexec/network/packet-steering.uc -l "$steering_flows" "$packet_steering"
fi
exec /usr/libexec/network/packet-steering.uc "$packet_steering"
