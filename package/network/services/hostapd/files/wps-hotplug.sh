if [ "$ACTION" = "pressed" -a "$BUTTON" = "wps" ]; then
	for dir in /var/run/hostapd-*; do
		[ -d "$dir" ] || continue
		hostapd_cli -p "$dir" wps_pbc
	done
fi
