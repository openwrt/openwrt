scan_pppoe() {
	scan_ppp "$@"
}

setup_interface_pppoe() {
	local iface="$1"
	local config="$2"
	
	config_get device "$config" device

	for module in slhc ppp_generic ppp_async ip_gre; do
		/sbin/insmod $module 2>&- >&-
	done
	setup_interface "$iface" "$config" "dhcp"

	config_get mtu "$cfg" mtu
	mtu=${mtu:-1452}
	start_pppd "$config" \
		plugin rp-pppoe.so \
		mtu $mtu mru $mtu \
		"nic-$device"
}
