scan_pptp() {
	scan_ppp "$@"
}

setup_interface_pptp() {
	local config="$2"
	local ifname
	
	config_get device "$config" device
	config_get ipproto "$config" ipproto

	for module in slhc ppp_generic ppp_async ip_gre; do
		/sbin/insmod $module 2>&- >&-
	done
	sleep 1

	setup_interface "$device" "$config" "${ipproto:-dhcp}"

	# fix up the netmask
	config_get netmask "$config" netmask
	[ -z "$netmask" -o -z "$device" ] || ifconfig $device netmask $netmask

	# make sure the network state references the correct ifname
	scan_ppp "$config"
	config_get ifname "$config" ifname
	uci set "/var/state/network.$config.ifname=$ifname"

	config_get mtu "$cfg" mtu
	config_get server "$cfg" server
	mtu=${mtu:-1452}
	start_pppd "$config" \
		pty "/usr/sbin/pptp $server --loglevel 0 --nolaunchpppd" \
		file /etc/ppp/options.pptp \
		mtu $mtu mru $mtu
}
