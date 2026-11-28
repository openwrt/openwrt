#!/bin/sh

packet_steering="$(uci -q get network.@globals[0].packet_steering)"
flow_offloading="$(uci -q get firewall.@defaults[0].flow_offloading)"
flow_offloading_hw="$(uci -q get firewall.@defaults[0].flow_offloading_hw)"

# DSA puts its sysfs group on each conduit. With the flow accelerator the
# switch has two, the traps' and the traffic's: take the one that the user
# ports hang off, the conduit with the most interfaces linked to it
conduit=eth0
users=0
for d in /sys/class/net/*/dsa; do
	[ -d "$d" ] || continue
	dev="${d%/dsa}"
	dev="${dev##*/}"
	ifindex="$(cat /sys/class/net/$dev/ifindex)"
	n=0
	for l in /sys/class/net/*/iflink; do
		[ "${l%/iflink}" = "/sys/class/net/$dev" ] && continue
		[ "$(cat "$l")" = "$ifindex" ] && n=$((n + 1))
	done
	[ $n -gt $users ] && conduit="$dev" users=$n
done

[ "$packet_steering" != 1 ] && {
	echo 0 > /sys/class/net/br-lan/queues/rx-0/rps_cpus
	echo 0 > /sys/class/net/$conduit/queues/rx-0/rps_cpus
	exit 0
}

. /lib/functions.sh

case "$(board_name)" in
meraki,mx65)
	# WAN uses the second CPU port (eth0), LAN the first one (eth1): give
	# each its own CPU, RPS on top of that only costs throughput
	for dev in eth0 eth1; do
		echo 0 > /sys/class/net/$dev/queues/rx-0/rps_cpus
	done
	echo 0 > /sys/class/net/br-lan/queues/rx-0/rps_cpus
	irq=$(awk '/ eth0$/ { sub(":", "", $1); print $1 }' /proc/interrupts)
	[ -n "$irq" ] && echo 2 > /proc/irq/$irq/smp_affinity
	exit 0
	;;
esac

if [ ${flow_offloading_hw:-0} -gt 0 ]; then
	# HW offloading
	echo 0 > /sys/class/net/br-lan/queues/rx-0/rps_cpus
	echo 0 > /sys/class/net/$conduit/queues/rx-0/rps_cpus
elif [ ${flow_offloading:-0} -gt 0 ]; then
	# SW offloading
	# br-lan setup doesn't seem to matter for offloading case
	echo 2 > /sys/class/net/$conduit/queues/rx-0/rps_cpus
else
	# Default
	echo 2 > /sys/class/net/br-lan/queues/rx-0/rps_cpus
	echo 0 > /sys/class/net/$conduit/queues/rx-0/rps_cpus
fi
