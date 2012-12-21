#!/bin/sh

[ "${ACTION}" = "released" ] || exit 0

. /lib/functions.sh

logger "$BUTTON pressed for $SEEN seconds"

local rfkill_state=0

wifi_rfkill_set() {
	uci set wireless.$1.disabled=$rfkill_state
}

wifi_rfkill_check() {
	local disabled
	config_get disabled $1 disabled
	[ "$disabled" = "1" ] || rfkill_state=1
}

case "${BUTTON}" in
	reset)
		if [ "$SEEN" -lt 1 ]
		then
			echo "REBOOT" > /dev/console
			sync
			reboot
		elif [ "$SEEN" -gt 5 ]
		then
			echo "FACTORY RESET" > /dev/console
			firstboot && reboot &
		fi
		;;

	wps)
		for dir in /var/run/hostapd-*; do
			[ -d "$dir" ] || continue
			hostapd_cli -p "$dir" wps_pbc
		done
		;;

	rfkill)
		config_load wireless
		config_foreach wifi_rfkill_check wifi-device
		config_foreach wifi_rfkill_set wifi-device
		uci commit wireless
		wifi up
		;;

	*)
		logger "unknown button ${BUTTON}"
		;;
esac
