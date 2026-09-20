#!/bin/sh
# econet-eth packet-steering platform override.
# packet_steering's reload_service() calls this instead of the stock
# packet-steering.uc when present -- the official extension point, used
# here instead of a separate init.d trigger so there's no race between two
# independent procd triggers on interface-up.
#
# Runs the stock steering logic unmodified, then re-applies eth0's RPS
# target to whichever CPU is NOT running econet-eth's RX-poll timer
# (simple_poll_cpu module parameter), which packet-steering.uc's own
# eth0 pick (always CPU0) would otherwise overwrite. Gated on the module
# parameter existing, not on board identity: this fixes an econet-eth
# driver characteristic (its simple-DMA RX-poll-timer design), not
# anything specific to one board's hardware, so it applies to every board
# using this driver's simple-DMA path -- the RPS_PATH/PARAM_PATH check
# below already no-ops cleanly on any board that lacks either.

steering_flows="$(uci -q get network.@globals[0].steering_flows)"
opts=""
[ "${steering_flows:-0}" -gt 0 ] 2>/dev/null && opts="-l $steering_flows"
/usr/libexec/network/packet-steering.uc $opts "$1"

RPS_PATH=/sys/class/net/eth0/queues/rx-0/rps_cpus
PARAM_PATH=/sys/module/econet_eth/parameters/simple_poll_cpu
if [ -f "$RPS_PATH" ] && [ -f "$PARAM_PATH" ]; then
	case "$(cat "$PARAM_PATH" 2>/dev/null)" in
	0) echo 2 > "$RPS_PATH" ;; # timer/IRQ on CPU0 -> RPS to CPU1
	1) echo 1 > "$RPS_PATH" ;; # timer/IRQ on CPU1 -> RPS to CPU0
	esac
fi
