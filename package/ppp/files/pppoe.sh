stop_interface_pppoe() {
	stop_interface_ppp "$1"
}

setup_interface_pppoe() {
	local iface="$1"
	local config="$2"
	
	for module in slhc ppp_generic pppox pppoe; do
		/sbin/insmod $module 2>&- >&-
	done

	config_get mtu "$config" mtu
	mtu=${mtu:-1492}
	start_pppd "$config" \
		"nic-$iface" \
		plugin rp-pppoe.so \
		mtu $mtu mru $mtu
}
