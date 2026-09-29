#!/bin/sh
# Shows the router's state on the TP-Link Archer BE550 front LED bar the way
# the stock firmware does. The effect is chosen by ledbar_choose in
# /etc/ledbar/choose.sh, a conffile kept across sysupgrade. SIGUSR1
# (/etc/init.d/ledbar reload) forces an immediate update.

trap : USR1

. /usr/share/libubox/jshn.sh
. /etc/ledbar/choose.sh

EFFECT=$(ls /sys/bus/i2c/drivers/tplink-ledbar/*/effect 2>/dev/null | head -n 1)
if [ -z "$EFFECT" ]; then
	logger -t ledbar "LED bar driver not bound, exiting"
	exit 0
fi

wan_up() {
	local reply up

	reply=$(ubus call network.interface.wan status 2>/dev/null)
	[ -n "$reply" ] || return 1
	json_load "$reply"
	json_get_var up up
	[ "$up" = 1 ]
}

# any hostapd BSS whose <method> reply has <field> = <value>
hostapd_any() {
	local obj reply val

	for obj in $(ubus list 'hostapd.*' 2>/dev/null); do
		reply=$(ubus call "$obj" "$1" 2>/dev/null)
		[ -n "$reply" ] || continue
		json_load "$reply"
		json_get_var val "$2"
		[ "$val" = "$3" ] && return 0
	done
	return 1
}

flag() {
	"$@" && echo 1 || echo 0
}

current=
failing=0

while :; do
	internet=$(flag wan_up)
	wifi=$(flag hostapd_any get_status status ENABLED)
	wps=$(flag hostapd_any wps_status pbc_status Active)
	[ "$(uci -q get ledbar.main.enabled)" = 0 ] && off=1 || off=0

	effect=$(ledbar_choose "$internet" "$wifi" "$wps" "$off")
	if [ "$effect" != "$current" ]; then
		if echo "$effect" > "$EFFECT" 2>/dev/null; then
			current=$effect
			failing=0
		elif [ "$failing" = 0 ]; then
			logger -t ledbar "failed to set effect $effect"
			failing=1
		fi
	fi

	sleep 2 &
	wait $!
done
