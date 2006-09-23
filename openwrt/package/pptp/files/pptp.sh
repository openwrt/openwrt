scan_pptp() {
	scan_ppp "$@"
}

setup_interface_pptp() {
	local iface="$1"
	local config="$2"
	
	config_get device "$config" device

	for module in slhc ppp_generic ppp_async ip_gre; do
		/sbin/insmod $module 2>&- >&-
	done
	setup_interface "$iface" "$config" "dhcp"

	config_get mtu "$cfg" mtu
	config_get server "$cfg" server
	mtu=${mtu:-1452}
	start_pppd "$config" \
		pty "/usr/sbin/pptp $server --loglevel 0 --nolaunchpppd"
		file /etc/ppp/options.pptp
		mtu $mtu mru $mtu
}
